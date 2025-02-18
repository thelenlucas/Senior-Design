#include "logs.hpp"

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>

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

// Gets a log from the database, given an ID
Log Log::fromID(uint id) {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(sq, "SELECT * FROM logs WHERE id = ?");
    query.bind(1, id);
    query.executeStep();
    return Log(
        query.getColumn("id"),
        query.getColumn("species"),
        query.getColumn("len_quarters"),
        query.getColumn("diameter_quarters"),
        query.getColumn("cost_cents_quarters"),
        query.getColumn("quality"),
        query.getColumn("location"),
        query.getColumn("notes")
    );
}

// Gets all logs from the database. This should
// only be used for static purposes, as it is not
// modifiable.
std::vector<Log> Log::logs() {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(sq, "SELECT * FROM logs");
    std::vector<Log> logs;
    
    while (query.executeStep()) {
        logs.push_back(Log(
            query.getColumn("id"),
            query.getColumn("species"),
            query.getColumn("len_quarters"),
            query.getColumn("diameter_quarters"),
            query.getColumn("cost_cents_quarters"),
            query.getColumn("quality"),
            query.getColumn("location"),
            query.getColumn("notes")
        ));
    }

    return logs;
}

// Inserts this item into the database
void Log::insert() {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(sq, "INSERT INTO logs (species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.bind(1, this->species);
    query.bind(2, this->len_quarters);
    query.bind(3, this->diameter_quarters);
    query.bind(4, this->cost_cents_quarters);
    query.bind(5, this->quality);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    query.exec();
}

// Removes this item from the database
void Log::remove() {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(sq, "DELETE FROM logs WHERE id = ?");
    query.bind(1, this->id);
    query.exec();
}

// Updates this item in the database
void Log::update() {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READWRITE);
    SQLite::Statement query(sq, "UPDATE logs SET species = ?, len_quarters = ?, diameter_quarters = ?, cost_cents_quarters = ?, quality = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, this->species);
    query.bind(2, this->len_quarters);
    query.bind(3, this->diameter_quarters);
    query.bind(4, this->cost_cents_quarters);
    query.bind(5, this->quality);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    query.bind(8, this->id);
    query.exec();
}

bool Log::isActivelyUsed() {
    auto sq = SQLite::Database(DATABASE_FILE, SQLite::OPEN_READONLY);
    SQLite::Statement query(sq, "SELECT l.*, COUNT(c.id) AS cutlist_count FROM logs l LEFT JOIN cutlist c ON c.log_id = l.id AND c.done <> 1 WHERE l.id = ? GROUP BY l.id");
    query.bind(1, this->id);
    query.executeStep();
    return (query.getColumn("cutlist_count").getInt() > 0);
}