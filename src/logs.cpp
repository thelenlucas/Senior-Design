#include "logs.hpp"
#include "types.hpp"

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>
#include <iostream>

// Schema:
// CREATE TABLE logs (
//     id                  INTEGER PRIMARY KEY AUTOINCREMENT,
//     species             VARCHAR NOT NULL,
//     len_quarters        INT     NOT NULL
//                                 CHECK ( (len_quarters > 0) ),
//     diameter_quarters   INT     NOT NULL
//                                 CHECK ( (diameter_quarters > 0) ),
//     cost_cents_quarters INT     NOT NULL
//                                 CHECK ( (cost_cents_quarters > 0) ),
//     quality             INTEGER CHECK ( (quality BETWEEN 1 AND 5) ),
//     location            TEXT    REFERENCES storage_bins (name),
//     notes               TEXT,
//     media               BLOB,
//     scrapped            INTEGER NOT NULL
//                                 DEFAULT (0) 
// );


Log::Log(int id,
    std::string species,
    uint len_quarters,
    uint diameter_quarters,
    uint cost_cents_quarters,
    uint quality,
    std::string location,
    std::string notes
) {
    this->id = id;
    this->species = species;
    this->len_quarters = len_quarters;
    this->diameter_quarters = diameter_quarters;
    this->cost_cents_quarters = cost_cents_quarters;
    this->quality = quality;
    this->location = location;
    this->notes = notes;
}

std::optional<Log> Log::get_by_id(int id) {
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM logs WHERE id = ?;");
    query.bind(1, id);
    if (query.executeStep()) {
        return Log(
            query.getColumn(0).getInt(),
            query.getColumn(1).getText(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getText(),
            query.getColumn(7).getText()
        );
    }
    return std::nullopt;
}

bool Log::insert() {
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(db, "INSERT INTO logs (species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?);");
    query.bind(1, this->species);
    query.bind(2, this->len_quarters);
    query.bind(3, this->diameter_quarters);
    query.bind(4, this->cost_cents_quarters);
    query.bind(5, this->quality);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    auto ret = query.exec() > 0;

    if (ret) {
        this->id = db.getLastInsertRowid();
    }

    // Cout exception if LOGS_LOGGING
    if (LOGS_LOGGING && !ret) {
        std::cout << "Failed to insert log into database" << std::endl;
    }

    return ret;
}

bool Log::update() {
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(db, "UPDATE logs SET species = ?, len_quarters = ?, diameter_quarters = ?, cost_cents_quarters = ?, quality = ?, location = ?, notes = ? WHERE id = ?;");
    query.bind(1, this->species);
    query.bind(2, this->len_quarters);
    query.bind(3, this->diameter_quarters);
    query.bind(4, this->cost_cents_quarters);
    query.bind(5, this->quality);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    query.bind(8, this->id);
    auto ret = query.exec() > 0;

    // Cout exception if LOGS_LOGGING
    if (LOGS_LOGGING && !ret) {
        std::cout << "Failed to update log in database" << std::endl;
    }

    return ret;
}

std::vector<Log> Log::get_all() {
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM logs;");
    std::vector<Log> logs;
    while (query.executeStep()) {
        logs.push_back(Log(
            query.getColumn(0).getInt(),
            query.getColumn(1).getText(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getText(),
            query.getColumn(7).getText()
        ));
    }
    return logs;
}

void Log::cut_length(uint amt) {
    //Remove length from Log
    this->len_quarters -= amt + 0.125;
    // Update Log
    Log::update();
}


void Log::scrap() {
    // Set the scrapped column to 1 in the database
    SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(db, "UPDATE logs SET scrapped = 1 WHERE id = ?;");
    query.bind(1, this->id);
    query.exec();

    if (LOGS_LOGGING) {
        std::cout << "Log " << this->id << " scrapped" << std::endl;
    }
}
