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
#include "db.hpp"
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

    // Database object for data
    std::optional<Database*> db;

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

    // Class function, takes in a database and returns all
    // slabs in the database as a vector
    static std::vector<Slab> allSlabs(Database *db);

    // Connects this slab to a database
    void connect(Database* db);

    // Removes this item from the database, throws an error if this is disconnected
    void remove();

    // Updates this item in the database, throws an error if this is disconnected
    void update();

    // Returns true if this slab is connected to a database
    bool isConnected() {return this->db.has_value();}
};