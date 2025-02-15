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

#include <string>
#include <stdexcept>
#include <optional>
#include "db.hpp"
#include "types.hpp"

#define COOKIES_LOGGING true

class Database;

class Cookie {
private:
    int id;
    std::string species;
    uint thickness_quarters;
    uint diameter_quarters;
    Drying drying;
    std::string location;
    std::string notes;

    // Database object for data
    std::optional<Database*> db;

public:
    Cookie(int id,
        std::string species,
        uint thickness_quarters,
        uint diameter_quarters,
        Drying drying,
        std::string location = "",
        std::string notes = "",
        std::optional<Database*> db = std::nullopt
    );

    // Getters
    int getId() {return id;}
    std::string getSpecies() {return species;}
    uint getThicknessQuarters() {return thickness_quarters;}
    uint getDiameterQuarters() {return diameter_quarters;}
    Drying getDrying() {return drying;}
    std::string getLocation() {return location;}
    std::string getNotes() {return notes;}

    // Connects this log to a database
    void connect(Database* db);

    // Removes this item from the database, throws an error if this is disconnected
    void remove();

    // Updates this item in the database, throws an error if this is disconnected
    void update();

    // Returns true if this log is connected to a database
    bool isConnected() {return this->db.has_value();}
};