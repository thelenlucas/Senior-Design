#ifndef LOGS_HPP
#define LOGS_HPP

#include <string>
#include <stdexcept>
#include <optional>
#include "db.hpp"

#define LOGS_LOGGING true

// CREATE TABLE cookies (
//     id                 INTEGER PRIMARY KEY AUTOINCREMENT
//                                UNIQUE
//                                NOT NULL,
//     species            TEXT    NOT NULL,
//     thickness_quarters INTEGER CHECK ( (thickness_quarters > 0) ) 
//                                NOT NULL,
//     diameter_quarters  INTEGER NOT NULL
//                                CHECK ( (diameter_quarters > 0) ),
//     drying             INTEGER NOT NULL
//                                CHECK ( (drying BETWEEN 0 AND 3) ),
//     location           VARCHAR,
//     notes              TEXT,
//     media              BLOB
// );

// Forward declaration
class Database;

class Log {
private:
    int id;
    std::string species;
    uint len_quarters;
    uint diameter_quarters;
    uint cost_cents_quarters;
    uint quality;
    std::string location;
    std::string notes;

    // Database object for data
    std::optional<Database*> db;

public:
    Log(int id,
        std::string species,
        uint len_quarters,
        uint diameter_quarters,
        uint cost_cents_quarters,
        uint quality,
        std::string location = "",
        std::string notes = ""
    );

    // Gets a log from the database, given an ID
    static Log fromID(uint id);

    // Inserts this item into the database
    void insert();

    // Static method to create a vector of logs from a database
    static std::vector<Log> logs();

    // Getters
    int getId() const {return id;} 
    std::string getSpecies() const {return species;} 
    uint getLenQuarters() const {return len_quarters;} 
    uint getDiameterQuarters() const {return diameter_quarters;} 
    uint getCostCentsQuarters() const {return cost_cents_quarters;} 
    uint getQuality() const {return quality;} 
    std::string getLocation() const {return location;} 
    std::string getNotes() const {return notes;}

    // Removes this item from the database
    void remove();

    // Updates this item in the database
    void update();

    // Returns true if this object is connected to a cutlist cut that isn't done yet
    bool isActivelyUsed();
};

#endif // TYPES_HPP