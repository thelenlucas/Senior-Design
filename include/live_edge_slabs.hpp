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

struct InProgressSlab {
    int width_eighths;
    int thickness_eighths;
    int kerf_eighths;
};

class Database;
class SlabManufacturer {
private:
    Log* original_log;
    int log_len_eighths;
    int log_diameter_eighths;
    int current_used_diameter_eighths;
    std::vector<InProgressSlab> madeCuts;
    std::vector<InProgressSlab> undoQueue;

    int widthAtOffsetFromCurrentEighths(int offset) {
        int midpoint = this->log_diameter_eighths / 2;
        int from_midpoint = abs(midpoint - this->current_used_diameter_eighths + offset);

        return 2 * sqrt(pow((this->log_diameter_eighths/2),2) - pow(from_midpoint, 2));
    }

public:
    SlabManufacturer(Log &log) {
        this->log_len_eighths = log.getLenQuarters() * 2;
        this->log_diameter_eighths = log.getDiameterQuarters() * 2;
        this->current_used_diameter_eighths = 8;
        madeCuts = std::vector<InProgressSlab>();
        undoQueue = std::vector<InProgressSlab>();
        this->original_log = &log;
    }

    int nextCutWouldBeEightsThick(int thickness_eighths) {
        int distance_from_current = this->widthAtOffsetFromCurrentEighths(0);
        int distance_from_cut = this->widthAtOffsetFromCurrentEighths(thickness_eighths);

        return (distance_from_cut + distance_from_current) / 2;
    }

    bool makeSlice(int thickness_eighths, int kerf_eights) {
        // Check if the cut is possible
        if (this->current_used_diameter_eighths + thickness_eighths + kerf_eights) {
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
    bool undo() {
        // Take the last cut, remove it from the madecuts and add it to the undo queue.
        if (this->madeCuts.size() == 0) {
            return false;
        }
        InProgressSlab lastCut = this->madeCuts.back();
        this->madeCuts.pop_back();
        this->undoQueue.push_back(lastCut);
        this->current_used_diameter_eighths -= lastCut.thickness_eighths + lastCut.kerf_eighths;
        return true;
    }
    
    // Returns false if there are no cuts to redo, true otherwise
    bool redo() {
        // Take the last cut added to the undo queue, remove it from the undo queue and add it to the made cuts.
        if (this->undoQueue.size() == 0) {
            return false;
        }

        InProgressSlab lastCut = this->undoQueue.back();
        this->undoQueue.pop_back();
        this->madeCuts.push_back(lastCut);
        this->current_used_diameter_eighths += lastCut.thickness_eighths + lastCut.kerf_eighths;
        return true;
    }

    int availableQuarters() {
        return this->original_log->getDiameterQuarters()*2 - this->current_used_diameter_eighths;
    };
};

class Slab {
private:
    int id;
    std::string species;
    uint thickness_quarters;
    uint len_quarters;
    Drying drying;
    bool smoothed;
    std::string location;
    std::string notes;

public:
    Slab(int id,
        std::string species,
        uint thickness_quarters,
        uint len_quarters,
        Drying drying,
        bool smoothed,
        std::string location = "",
        std::string notes = "",
        std::optional<Database*> db = std::nullopt
    );

    // Getters
    int getId() {return id;}
    std::string getSpecies() {return species;}
    uint getThicknessQuarters() {return thickness_quarters;}
    uint getLenQuarters() {return len_quarters;}
    Drying getDrying() {return drying;}
    bool getSmoothed() {return smoothed;}
    std::string getLocation() {return location;}
    std::string getNotes() {return notes;}

    // Make from a log and a InProgressSlab
    static Slab fromLogAndSlab(Log &log, InProgressSlab &slab, std::optional<Database*> db = std::nullopt) {
        return Slab(0, log.getSpecies(), slab.thickness_eighths / 2, slab.width_eighths / 2, log.getDrying(), false);
    }
};