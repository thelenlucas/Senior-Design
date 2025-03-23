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

#define SLABS_LOGGING true

class Database;

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
};