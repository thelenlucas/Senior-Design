#ifndef LIVE_EDGE_SLABS_HPP // Added include guard
#define LIVE_EDGE_SLABS_HPP

#include <string>
#include <stdexcept>
#include <optional>
#include "types.hpp"
#include <vector>
#include "logs.hpp"
#include <cmath>
#include <iostream> // Added for logging
#include <SQLiteCpp/SQLiteCpp.h> // Added for database access
#include "manufacturable.hpp" // Added for inheritance

#define SLABS_LOGGING true

struct InProgressSlab
{
    int width_eighths;
    int thickness_eighths;
    int kerf_eighths;
};

// Removed forward declaration for Database as SQLiteCpp.h is included

// DDL:
// CREATE TABLE slabs (...) - Kept for reference

// Changed inheritance to Manufacturable<Slab>
class Slab : public Manufacturable<Slab>
{
private:
    int id;
    std::string species;
    uint thickness_eights; // Renamed from thickness_quarters in constructor? Check definition
    uint len_quarters;
    uint width_eighths;
    Drying drying;
    bool smoothed;
    std::string location;
    std::string notes;

    // Removed private InsertSlab() and UpdateSlab() declarations
    // Removed private static from_log() declaration (logic moved to static make_from_log)

public:
    // Constructor - signature matches cpp file, but thickness param name differs
    Slab(int id,
         std::string species,
         uint thickness_eights, // Matched name to member variable
         uint len_quarters,     // Matched name to member variable
         uint width_eighths,
         Drying drying,
         bool smoothed,
         std::string location = "",
         std::string notes = "");

    // Removed InsertOrUpdate() declaration

    // Renamed CutAllSlabs to manufacture_and_persist_slabs
    // This method handles the specific logic using InProgressSlab,
    // distinct from the standard Manufacturable::make_from_log
    static std::vector<Slab> manufacture_and_persist_slabs(Log &log, std::vector<InProgressSlab> slabs, int len_quarters, std::string location = "", std::string notes = "");

    // --- Getters ---
    std::string getSpecies() const { return species; } // Added const
    uint getThicknessEights() const { return thickness_eights; } // Added const
    uint getLenQuarters() const { return len_quarters; } // Added const
    uint getWidthEighths() const { return width_eighths; } // Added width getter
    Drying getDrying() const { return drying; } // Added const
    bool getSmoothed() const { return smoothed; } // Added const
    std::string getLocation() const { return location; } // Added const
    std::string getNotes() const { return notes; } // Added const

    // --- Persistent Interface Implementation ---
    int get_id() const override { return id; }
    bool insert() override; // Declaration added
    bool update() override; // Declaration added
    static std::optional<Slab> get_by_id(int id); // Declaration added
    static std::vector<Slab> get_all(); // Declaration added

    // --- Manufacturable Interface Implementation ---
    // Implements the static method required by Manufacturable<Slab> base class.
    // Creates a single Slab object (without persisting).
    // Note: The base Manufacturable::make_from_log calls this and then product.insert().
    static std::vector<Slab> make_from_log(
        Log log,
        int len_quarters, // Represents the desired length of the slab
        std::optional<int> thickness_eights = std::nullopt,
        std::optional<int> width_eights = std::nullopt, // Width might not be known ahead of time for live edge
        std::optional<Drying> drying = std::nullopt
    );

};

class SlabManufacturer
{
private:
    Log *original_log;
    int log_len_quarters; // Changed name for clarity
    int log_diameter_eighths;
    int current_used_diameter_eighths;
    std::vector<InProgressSlab> madeCuts;
    std::vector<InProgressSlab> undoQueue;

    int widthAtOffsetFromCurrentEighths(int offset)
    {
        // Check if calculation is possible
        double radius = this->log_diameter_eighths / 2.0;
        double midpoint = radius;
        // Calculate distance from the center to the point where the cut starts *after* the offset
        double dist_from_midpoint = abs(midpoint - (this->current_used_diameter_eighths + offset));


        if (dist_from_midpoint >= radius) {
             if (SLABS_LOGGING) std::cout << "widthAtOffsetFromCurrentEighths: Offset " << offset << " is outside or at the edge of the log radius (" << radius <<"). Distance from midpoint: " << dist_from_midpoint << std::endl;
            return 0; // Offset is outside the log diameter
        }

        // Calculate half-width using Pythagorean theorem: (radius^2 - dist_from_midpoint^2) = (half_width^2)
        double half_width_squared = pow(radius, 2) - pow(dist_from_midpoint, 2);
         if (half_width_squared < 0) { // Should not happen if dist_from_midpoint < radius, but check anyway
             if (SLABS_LOGGING) std::cout << "widthAtOffsetFromCurrentEighths: Negative value in sqrt calculation." << std::endl;
             return 0;
         }
        return static_cast<int>(2.0 * sqrt(half_width_squared));
    }


public:
    SlabManufacturer(Log &log)
    {
        this->original_log = &log; // Store pointer
        this->log_len_quarters = log.getLenQuarters();
        this->log_diameter_eighths = log.getDiameterQuarters() * 2; // Convert quarters to eighths
        // Start from edge, assuming first cut removes 1 inch (8 eighths) - adjust if needed
        this->current_used_diameter_eighths = 0; // Start from 0 diameter used
        madeCuts = std::vector<InProgressSlab>();
        undoQueue = std::vector<InProgressSlab>();

        if (SLABS_LOGGING) {
             std::cout << "SlabManufacturer created for log ID " << log.get_id()
                       << ", Length: " << log_len_quarters << " quarters"
                       << ", Diameter: " << log_diameter_eighths << " eighths" << std::endl;
         }
    }

    // Calculates the approximate width of the *next* slab if cut with thickness_eighths
    int nextCutWidthEighths(int thickness_eighths)
    {
         // Calculate width at the start and end points of the cut thickness
         // Width at start of cut (current diameter usage)
         int width_start = this->widthAtOffsetFromCurrentEighths(0);
         // Width at end of cut (after thickness is used)
         int width_end = this->widthAtOffsetFromCurrentEighths(thickness_eighths);

         if (SLABS_LOGGING) {
             std::cout << "nextCutWidthEighths(thick=" << thickness_eighths
                       << "): width_start=" << width_start
                       << " (at diameter " << current_used_diameter_eighths << ")"
                       << ", width_end=" << width_end
                       << " (at diameter " << current_used_diameter_eighths + thickness_eighths << ")"
                       << std::endl;
         }
         // Return the average width (approximation for a trapezoid/rectangle)
         // If either width is 0, it means part of the cut is outside the log, return 0 or min?
         // Let's return average, but handle 0 width in makeSlice.
         if (width_start <= 0 || width_end <= 0) return 0; // Can't cut if start or end is outside
         return (width_start + width_end) / 2;
    }


    bool makeSlice(int thickness_eighths, int kerf_eights)
    {
        if (thickness_eighths <= 0 || kerf_eights < 0) {
             if (SLABS_LOGGING) std::cout << "Slice failed: Invalid dimensions (thickness=" << thickness_eighths << ", kerf=" << kerf_eights << ")" << std::endl;
            return false;
        }
        // Check if the cut is possible within the remaining diameter
        int needed_diameter = thickness_eighths + kerf_eights;
        int available_diameter = this->log_diameter_eighths - this->current_used_diameter_eighths;
        if (needed_diameter > available_diameter)
        {
             if (SLABS_LOGGING) {
                 std::cout << "Slice failed: Not enough diameter remaining. Needed: "
                           << needed_diameter
                           << ", Available: " << available_diameter
                           << std::endl;
             }
            return false;
        }

        // Get the width of the cut board
        int board_width = this->nextCutWidthEighths(thickness_eighths);
        if (board_width <= 0) {
             if (SLABS_LOGGING) {
                 std::cout << "Slice failed: Calculated board width is zero or negative (" << board_width << ")." << std::endl;
             }
            return false; // Cannot make a slice with no width
        }

        InProgressSlab slab = {board_width, thickness_eighths, kerf_eights};
        this->madeCuts.push_back(slab);
        // Advance the used diameter by the thickness of the board and the kerf
        this->current_used_diameter_eighths += needed_diameter;

        if (SLABS_LOGGING) {
            std::cout << "Slice made: Thickness=" << thickness_eighths << "/8", Width=" << board_width << "/8", Kerf=" << kerf_eights << "/8"" << std::endl;
            std::cout << "Total diameter used: " << this->current_used_diameter_eighths << "/8" out of " << this->log_diameter_eighths << "/8"" << std::endl;
        }

        // Clear the redo queue on a new action
        this->undoQueue.clear();
        return true;
    }

    bool undo()
    {
        if (this->madeCuts.empty())
        {
             if (SLABS_LOGGING) std::cout << "Undo failed: No cuts to undo." << std::endl;
            return false;
        }
        InProgressSlab lastCut = this->madeCuts.back();
        this->madeCuts.pop_back();
        this->undoQueue.push_back(lastCut);
        // Reclaim the diameter used by the undone cut
        this->current_used_diameter_eighths -= (lastCut.thickness_eighths + lastCut.kerf_eighths);
        if (SLABS_LOGGING) std::cout << "Undo: Thickness=" << lastCut.thickness_eighths << ", Kerf=" << lastCut.kerf_eighths << ". Diameter used now: " << current_used_diameter_eighths << std::endl;
        return true;
    }

    bool redo()
    {
         if (this->undoQueue.empty())
        {
             if (SLABS_LOGGING) std::cout << "Redo failed: No cuts to redo." << std::endl;
            return false;
        }
        InProgressSlab lastUndo = this->undoQueue.back();
        // Check if redone cut still fits (should always if undo/redo logic is correct)
        int needed_diameter = lastUndo.thickness_eighths + lastUndo.kerf_eighths;
         int available_diameter = this->log_diameter_eighths - this->current_used_diameter_eighths;
         if (needed_diameter > available_diameter) {
             if (SLABS_LOGGING) std::cout << "Redo failed: Not enough diameter remaining (this shouldn't happen)." << std::endl;
             // Clear redo queue if inconsistent state detected?
             this->undoQueue.clear();
             return false;
         }

        this->undoQueue.pop_back();
        this->madeCuts.push_back(lastUndo);
        // Re-apply the diameter used by the redone cut
        this->current_used_diameter_eighths += needed_diameter;
         if (SLABS_LOGGING) std::cout << "Redo: Thickness=" << lastUndo.thickness_eighths << ", Kerf=" << lastUndo.kerf_eighths << ". Diameter used now: " << current_used_diameter_eighths << std::endl;
        return true;
    }

     // Returns remaining diameter in eighths
    int availableDiameterEighths() const // Added const
    {
        return this->log_diameter_eighths - this->current_used_diameter_eighths;
    }

    // Returns vector of currently defined cuts
    const std::vector<InProgressSlab>& getMadeCuts() const { // Added const getter
        return madeCuts;
    }

     // Returns vector of cuts available for redo
     const std::vector<InProgressSlab>& getUndoQueue() const { // Added const getter
         return undoQueue;
     }

    // Produce all the cuts and persist them to the database.
    // Uses the renamed static method in Slab.
    std::vector<Slab> finalize(std::string location = "", std::string notes = "")
    {
        if (!original_log) {
             throw std::runtime_error("SlabManufacturer has no associated log for finalization.");
        }
        if (madeCuts.empty()) {
            if (SLABS_LOGGING) std::cout << "Finalize: No cuts made, returning empty vector." << std::endl;
            return {}; // Return empty vector if no cuts were made
        }

        if (SLABS_LOGGING) std::cout << "Finalizing " << madeCuts.size() << " slab cuts from log " << original_log->get_id() << " with length " << log_len_quarters << " quarters." << std::endl;


        // Use the renamed static method from Slab class
        // Pass the length of the *original* log for the slabs
        std::vector<Slab> slabs = Slab::manufacture_and_persist_slabs(*this->original_log, this->madeCuts, this->log_len_quarters, location, notes);

        // Clear the state of the manufacturer *after* successful operation
        this->madeCuts.clear();
        this->undoQueue.clear();
        // Keep current_used_diameter_eighths as is? Or reset?
        // Resetting seems appropriate if finalize consumes the manufacturer state.
        this->current_used_diameter_eighths = 0;

        // Note: Log length reduction happens inside manufacture_and_persist_slabs

        if (SLABS_LOGGING) std::cout << "Finalize complete. Manufacturer state cleared. Returning " << slabs.size() << " persisted slabs." << std::endl;

        return slabs;
    }
};

#endif // LIVE_EDGE_SLABS_HPP
