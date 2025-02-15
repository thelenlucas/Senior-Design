#include "db.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include <iostream>

using namespace std;

/// LOGS
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


void Database::deleteLog(Log log) {
    cout << "DATABASE: Deleting log with ID " << log.getId() << endl;

    SQLite::Statement query(this->db, "DELETE FROM logs WHERE id = ?");
    query.bind(1, log.getId());
    query.exec();
}

void Database::updateLog(Log log) {
    cout << "DATABASE: Updating log with ID " << log.getId() << endl;

    // Logs table
    SQLite::Statement query(this->db, "UPDATE logs SET species = ?, len_quarters = ?, diameter_quarters = ?, cost_cents_quarters = ?, quality = ?, location = ?, notes = ? WHERE id = ?");

}

vector<Log> Database::allLogs() {
    cout << "DATABASE: Getting all logs" << endl;

    vector<Log> logs;
    SQLite::Statement query(this->db, "SELECT id, species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes FROM logs");
    while (query.executeStep()) {
        Log log = Log(
            query.getColumn(0).getInt(),
            query.getColumn(1).getText(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getText(),
            query.getColumn(7).getText(),
            std::optional<Database*>(this)
        );
        logs.push_back(log);
    }

    return logs;
}

Log Database::getLog(uint id) {
    cout << "DATABASE: Getting log with ID " << id << endl;

    SQLite::Statement query(this->db, "SELECT species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes FROM logs WHERE id = ?");
    query.bind(1, id);
    query.executeStep();

    return Log(
        id,
        query.getColumn(0).getText(),
        query.getColumn(1).getInt(),
        query.getColumn(2).getInt(),
        query.getColumn(3).getInt(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getText(),
        query.getColumn(6).getText(),
        std::optional<Database*>(this)
    );
}

Log Database::insertLog(Log log) {
    cout << "DATABASE: Inserting log with properties: " << log.getSpecies() << ", " << log.getLenQuarters() << ", " << log.getDiameterQuarters() << ", " << log.getQuality() << ", " << log.getLocation() << endl;

    SQLite::Statement query(this->db, "INSERT INTO logs (species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.bind(1, log.getSpecies());
    query.bind(2, log.getLenQuarters());
    query.bind(3, log.getDiameterQuarters());
    query.bind(4, log.getCostCentsQuarters());
    query.bind(5, log.getQuality());
    query.bind(6, log.getLocation());
    query.bind(7, log.getNotes());
    query.exec();

    return log;
}