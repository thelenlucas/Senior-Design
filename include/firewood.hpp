// CREATE TABLE firewood (
//     id       INTEGER PRIMARY KEY AUTOINCREMENT
//                      UNIQUE
//                      NOT NULL,
//     species  TEXT    NOT NULL,
//     drying   INTEGER NOT NULL
//                      CHECK ( (drying BETWEEN 0 AND 3) ),
//     feet_3   INTEGER CHECK ( (feet_3 >= 0) ) 
//                      NOT NULL,
//     location VARCHAR,
//     notes    TEXT,
//     media    BLOB
// );

#include <string>
#include <stdexcept>
#include <optional>
#include "db.hpp"
#include "types.hpp"

#define FIREWOOD_LOGGING true

class Database;

class Firewood
{
private:
    int id;
    std::string species;
    Drying drying;
    uint feet_3;
    std::string location;
    std::string notes;

    // Database object for data
    std::optional<Database *> db;    

public:
    Firewood(
        int id,
        std::string species,
        Drying drying,
        uint feet_3,
        std::string location = "",
        std::string notes = "",
        std::optional<Database *> db = std::nullopt
    );

    // Class function, takes in a database and returns all
    // firewood in the database as a vector
    static std::vector<Firewood> allFirewood(Database *db);

    // Getters
    int getId() { return id; }
    std::string getSpecies() { return species; }
    Drying getDrying() { return drying; }
    uint getFeet3() { return feet_3; }
    std::string getLocation() { return location; }
    std::string getNotes() { return notes; }

    // One chord = 128 cubic feet
    float getChords() { return feet_3 / 128.0; }

    // Connects this log to a database
    void connect(Database *db);

    // Removes this item from the database, throws an error if this is disconnected
    void remove();

    // Updates this item in the database, throws an error if this is disconnected
    void update();

    // Returns true if connected to a database, false otherwise
    bool isConnected() { return db.has_value(); }
};
