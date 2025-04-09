// CREATE TABLE slabs (
//     id                 INTEGER PRIMARY KEY AUTOINCREMENT
//                                UNIQUE
//                                NOT NULL,
//     species            TEXT    NOT NULL,
//     thickness_quarters INTEGER CHECK ( (thickness_quarters > 0) )
//                                NOT NULL,
//     len_quarters       INTEGER NOT NULL
//                                CHECK ( (len_quarters > 0) ),
//     drying             INTEGER NOT NULL
//                                CHECK ( (drying BETWEEN 0 AND 3) ),
//     smoothed           INTEGER CHECK ( (smoothed BETWEEN 0 AND 1) )
//                                NOT NULL,
//     location           VARCHAR,
//     notes              TEXT,
//     media              BLOB
// );

#include <string>
#include <stdexcept>
#include <optional>
#include "types.hpp"
#include <vector>
#include "logs.hpp"
#include <cmath>

#define SLABS_LOGGING true

struct InProgressSlab
{
    int width_eighths;
    int thickness_eighths;
    int kerf_eighths;
};

class Database;
class SlabManufacturer
{
private:
    Log *original_log;
    int log_len_eighths;
    int log_diameter_eighths;
    int current_used_diameter_eighths;
    std::vector<InProgressSlab> madeCuts;
    std::vector<InProgressSlab> undoQueue;

    int widthAtOffsetFromCurrentEighths(int offset)
    {
        int midpoint = this->log_diameter_eighths / 2;
        int from_midpoint = abs(midpoint - this->current_used_diameter_eighths + offset);

        return 2 * sqrt(pow((this->log_diameter_eighths / 2), 2) - pow(from_midpoint, 2));
    }

public:
    SlabManufacturer(Log &log)
    {
        this->log_len_eighths = log.getLenQuarters() * 2;
        this->log_diameter_eighths = log.getDiameterQuarters() * 2;
        this->current_used_diameter_eighths = 8;
        madeCuts = std::vector<InProgressSlab>();
        undoQueue = std::vector<InProgressSlab>();
        this->original_log = &log;
    }

    int nextCutWouldBeEightsThick(int thickness_eighths)
    {
        int distance_from_current = this->widthAtOffsetFromCurrentEighths(0);
        int distance_from_cut = this->widthAtOffsetFromCurrentEighths(thickness_eighths);

        return (distance_from_cut + distance_from_current) / 2;
    }

    bool makeSlice(int thickness_eighths, int kerf_eights)
    {
        // Check if the cut is possible
        if (this->current_used_diameter_eighths + thickness_eighths + kerf_eights)
        {
            return false;
        }

        // Get the width of the cut board
        int board_width = this->nextCutWouldBeEightsThick(thickness_eighths);
        InProgressSlab slab = {board_width, thickness_eighths, kerf_eights};
        this->madeCuts.push_back(slab);
        this->current_used_diameter_eighths += thickness_eighths + kerf_eights;

        return true;
    }

    // Returns false if there are no cuts to undo, true otherwise
    bool undo()
    {
        // Take the last cut, remove it from the madecuts and add it to the undo queue.
        if (this->madeCuts.size() == 0)
        {
            return false;
        }
        InProgressSlab lastCut = this->madeCuts.back();
        this->madeCuts.pop_back();
        this->undoQueue.push_back(lastCut);
        this->current_used_diameter_eighths -= lastCut.thickness_eighths + lastCut.kerf_eighths;
        return true;
    }

    // Returns false if there are no cuts to redo, true otherwise
    bool redo()
    {
        // Take the last cut added to the undo queue, remove it from the undo queue and add it to the made cuts.
        if (this->undoQueue.size() == 0)
        {
            return false;
        }

        InProgressSlab lastCut = this->undoQueue.back();
        this->undoQueue.pop_back();
        this->madeCuts.push_back(lastCut);
        this->current_used_diameter_eighths += lastCut.thickness_eighths + lastCut.kerf_eighths;
        return true;
    }

    int availableQuarters()
    {
        return this->original_log->getDiameterQuarters() * 2 - this->current_used_diameter_eighths;
    };

    // Produce all of the cuts using the static functions in Slab
    // and insert them into the database
    std::vector<Slab> finalize(std::string location = "", std::string notes = "")
    {
        // Simply shove all of our in progress logs into CutAllSlabs
        std::vector<Slab> slabs = Slab::CutAllSlabs(*this->original_log, this->madeCuts, this->log_len_eighths, location, notes);

        // Clear the made cuts and undo queue
        this->madeCuts.clear();
        this->undoQueue.clear();

        // Return the slabs
        return slabs;
    }

};

// DDL:
// CREATE TABLE slabs (
//     id               INTEGER PRIMARY KEY AUTOINCREMENT
//                              UNIQUE
//                              NOT NULL,
//     from_log         INTEGER REFERENCES logs (id),
//     species          TEXT    NOT NULL,
//     thickness_eights INTEGER CHECK ( (thickness_eights > 0) )
//                              NOT NULL,
//     len_quarters     INTEGER NOT NULL
//                              CHECK ( (len_quarters > 0) ),
//     drying           INTEGER NOT NULL
//                              CHECK ( (drying BETWEEN 0 AND 3) ),
//     smoothed         INTEGER CHECK ( (smoothed BETWEEN 0 AND 1) )
//                              NOT NULL,
//     location         TEXT    REFERENCES storage_bins (name),
//     notes            TEXT,
//     media            BLOB,
//     cut              INTEGER REFERENCES partial_cuts (id),
//     width_eights     INTEGER NOT NULL
//                              DEFAULT (1)
// );

class Slab
{
private:
    int id;
    std::string species;
    uint thickness_eights;
    uint len_quarters;
    uint width_eighths;
    Drying drying;
    bool smoothed;
    std::string location;
    std::string notes;

    // Insert this item into the database. This is a private abstraction
    void InsertSlab()
    {
        if (this->id >= 0)
        {
            throw std::runtime_error("Slab ID is non-negative, cannot insert");
        }

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
        SQLite::Statement query(db, "INSERT INTO slabs (species, thickness_eights, len_quarters, drying, smoothed, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?);");
        query.bind(1, this->species);
        query.bind(2, this->thickness_eights);
        query.bind(3, this->len_quarters);
        query.bind(4, static_cast<int>(this->drying));
        query.bind(5, this->smoothed);
        query.bind(6, this->location);
        query.bind(7, this->notes);

        auto ret = query.exec() > 0;

        if (ret)
        {
            this->id = db.getLastInsertRowid();
        }

        if (SLABS_LOGGING && !ret)
        {
            std::cout << "Failed to insert slab into database" << std::endl;
        }
    }

    // Update this item if it exists in the database (we have a non-negative id)
    void UpdateSlab()
    {
        if (this->id < 0)
        {
            throw std::runtime_error("Slab ID is negative, cannot update");
        }

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
        SQLite::Statement query(db, "UPDATE slabs SET species = ?, thickness_eights = ?, len_quarters = ?, drying = ?, smoothed = ?, location = ?, notes = ? WHERE id = ?;");
        query.bind(1, this->species);
        query.bind(2, this->thickness_eights);
        query.bind(3, this->len_quarters);
        query.bind(4, static_cast<int>(this->drying));
        query.bind(5, this->smoothed);
        query.bind(6, this->location);
        query.bind(7, this->notes);
        query.bind(8, this->id);

        auto ret = query.exec() > 0;

        if (SLABS_LOGGING && !ret)
        {
            std::cout << "Failed to update slab in database" << std::endl;
        }
    }

    // Make from a log, private because this doesn't actually cut the log.
    // Don't want to cut it accidentally
    static Slab from_log(Log &log, InProgressSlab &slab, int len_quarters, std::string location = "", std::string notes = "")
    {
        // Leave rough initially
        bool smoothed = false;
        // Get the species, drying from the log
        std::string species = log.getSpecies();
        Drying drying = log.getDrying();

        // Dimension from inprogress + given length
        uint thickness_eights = slab.thickness_eighths;
        uint width_eighths = slab.width_eighths;

        // Create the slab
        Slab newSlab(-1, species, thickness_eights, width_eighths, len_quarters, drying, smoothed, location, notes);

        return newSlab;
    }

public:
    Slab(int id,
         std::string species,
         uint thickness_quarters,
         uint len_eights,
         uint width_eighths,
         Drying drying,
         bool smoothed,
         std::string location = "",
         std::string notes = "");

    // Dynamically inserts or updates a slab in the database
    void InsertOrUpdate()
    {
        if (this->id < 0)
        {
            this->InsertSlab();
        }
        else
        {
            this->UpdateSlab();
        }
    }

    // Constructs a vector of slabs, iteratively from from_log. Then removes the stated length from the log
    // and inserts the slabs into the database.
    // Returns a vector of slabs
    static std::vector<Slab> CutAllSlabs(Log &log, std::vector<InProgressSlab> slabs, int len_quarters, std::string location = "", std::string notes = "")
    {
        std::vector<Slab> slabVector;
        for (auto &slab : slabs)
        {
            Slab newSlab = Slab::from_log(log, slab, len_quarters, location, notes);
            newSlab.InsertOrUpdate();
            slabVector.push_back(newSlab);
        }

        // Update the log
        log.cut_length(len_quarters);

        return slabVector;
    }

    // Getters
    int getId() { return id; }
    std::string getSpecies() { return species; }
    uint getThicknessEights() { return thickness_eights; }
    uint getLenQuarters() { return len_quarters; }
    Drying getDrying() { return drying; }
    bool getSmoothed() { return smoothed; }
    std::string getLocation() { return location; }
    std::string getNotes() { return notes; }
};