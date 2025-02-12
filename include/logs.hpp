#ifndef LOGS_HPP
#define LOGS_HPP

#include <cstdint>
#include <string>
#include <stdexcept>
#include <optional>

#include "db.hpp"

#define LOGS_LOGGING true

// CREATE TABLE logs (
//     id                  INTEGER PRIMARY KEY AUTOINCREMENT,
//     species             VARCHAR NOT NULL,
//     count               INTEGER NOT NULL
//                                 CHECK ( (count > -1) ),
//     len_quarters        INT     NOT NULL
//                                 CHECK ( (len_quarters > 0) ),
//     diameter_quarters   INT     NOT NULL
//                                 CHECK ( (diameter_quarters > 0) ),
//     cost_cents_quarters INT     NOT NULL
//                                 CHECK ( (cost_cents_quarters > 0) ),
//     quality             INTEGER CHECK ( (quality BETWEEN 1 AND 5) ),
//     location            VARCHAR,
//     notes               TEXT,
//     media               BLOB
// );

// Forward declaration
class Database;

class Log {
private:
    int id;
    std::string species;
    std::uint32_t count;
    std::uint32_t len_quarters;
    std::uint32_t diameter_quarters;
    std::uint32_t cost_cents_quarters;
    std::uint32_t quality;
    std::string location;
    std::string notes;

    // Database object for data
    std::optional<Database*> db;

public:
    Log(int id,
        std::string species,
        std::uint32_t count,
        std::uint32_t len_quarters,
        std::uint32_t diameter_quarters,
        std::uint32_t cost_cents_quarters,
        std::uint32_t quality,
        std::string location = "",
        std::string notes = "",
        std::optional<Database*> db = std::nullopt
    );

    // Getters
    int getId() {return id;}
    std::string getSpecies() {return species;}
    std::uint32_t getCount() {return count;}
    std::uint32_t getLenQuarters() {return len_quarters;}
    std::uint32_t getDiameterQuarters() {return diameter_quarters;}
    std::uint32_t getCostCentsQuarters() {return cost_cents_quarters;}
    std::uint32_t getQuality() {return quality;}
    std::string getLocation() {return location;}
    std::string getNotes() {return notes;}
    std::optional<Database*> getDatabase() {return db;}

    // ID Setter, should be only used by the database, and with caution
    void setID(int id) {this->id = id;}

    // Connects this log to a database
    void connect(Database* db);

    // Returns true if connected to a database, false otherwise
    bool isConnected() {return db.has_value();}

    // Adds another log pile to this pile
    void combine(Log other);

    // Removes this item from the database, throws an error if this is disconnected
    void remove();

    // Updates this item in the database, throws an error if this is disconnected
    void update();
};

#endif // LOGS_HPP