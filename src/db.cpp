#include "db.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "logs.hpp"
#include <iostream>

using namespace std;

void Database::deleteLog(Log log) {
    cout << "DATABASE: Deleting log with ID " << log.getId() << endl;
    SQLite::Statement query(this->db, "DELETE FROM logs WHERE id = ?");
    query.bind(1, log.getId());
    query.exec();
}

void Database::updateLog(Log log) {
    cout << "DATABASE: Updating log with ID " << log.getId() << endl;

    // Logs table
    SQLite::Statement query(this->db, "UPDATE logs SET species = ?, count = ?, len_quarters = ?, diameter_quarters = ?, cost_cents_quarters = ?, quality = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, log.getSpecies());
    query.bind(2, log.getCount());
    query.bind(3, log.getLenQuarters());
    query.bind(4, log.getDiameterQuarters());
    query.bind(5, log.getCostCentsQuarters());
    query.bind(6, log.getQuality());
    query.bind(7, log.getLocation());
    query.bind(8, log.getNotes());
    query.bind(9, log.getId());
    query.exec();
    SQLite::Statement query2(this->db, "CREATE UNIQUE INDEX IF NOT EXISTS ExtantLog ON logs (species, len_quarters, diameter_quarters, quality, location)");
}

vector<Log> Database::allLogs() {
    cout << "DATABASE: Getting all logs" << endl;

    vector<Log> logs;
    SQLite::Statement query(this->db, "SELECT id, species, count, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes FROM logs");
    while (query.executeStep()) {
        logs.push_back(Log(
            query.getColumn(0).getInt(),
            query.getColumn(1).getText(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getInt(),
            query.getColumn(7).getText(),
            query.getColumn(8).getText(),
            this
        ));
    }

    return logs;
}

Log Database::getLog(uint id) {
    cout << "DATABASE: Getting log with ID " << id << endl;

    SQLite::Statement query(this->db, "SELECT species, count, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes FROM logs WHERE id = ?");
    query.bind(1, id);
    query.executeStep();

    return Log(
        id,
        query.getColumn(0).getText(),
        query.getColumn(1).getInt(),
        query.getColumn(2).getInt(),
        query.getColumn(3).getInt(),
        query.getColumn(4).getInt(),
        query.getColumn(5).getInt(),
        query.getColumn(6).getText(),
        query.getColumn(7).getText(),
        this
    );
}

std::optional<Log> Database::findLog(Log log) {
    cout << "DATABASE: Finding log with properties: " << log.getSpecies() << ", " << log.getLenQuarters() << ", " << log.getDiameterQuarters() << ", " << log.getQuality() << ", " << log.getLocation() << endl;

    // First, get if a duplicate exists by
    // species, len_quarters, diameter_quarters, quality, and location
    // Limit to 1, this is a unique index
    SQLite::Statement query(this->db, "SELECT id FROM logs WHERE species = ? AND len_quarters = ? AND diameter_quarters = ? AND quality = ? AND location = ? LIMIT 1");
    query.bind(1, log.getSpecies());
    query.bind(2, log.getLenQuarters());
    query.bind(3, log.getDiameterQuarters());
    query.bind(4, log.getQuality());
    query.bind(5, log.getLocation());
    query.executeStep();

    if (query.hasRow()) {
        cout << "DATABASE: Found log with ID " << query.getColumn(0).getInt() << endl;
        return this->getLog(query.getColumn(0).getInt());
    } else {
        cout << "DATABASE: Log not found" << endl;
        return std::nullopt;
    }
}

Log Database::insertLog(Log log) {
    cout << "DATABASE: Inserting log with properties: " << log.getSpecies() << ", " << log.getCount() << ", " << log.getLenQuarters() << ", " << log.getDiameterQuarters() << ", " << log.getCostCentsQuarters() << ", " << log.getQuality() << ", " << log.getLocation() << ", " << log.getNotes() << endl;

    // First search and see if this log already exists
    std::optional<Log> extant = this->findLog(log);

    // If it does, combine the logs
    if (extant.has_value()) {
        cout << "DATABASE: Log already exists, combining logs" << endl;
        extant.value().combine(log);
        return extant.value();
    }

    // If not, connect this log to the database, and insert it
    log.connect(this);
    SQLite::Statement query(this->db, "INSERT INTO logs (species, count, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.bind(1, log.getSpecies());
    query.bind(2, log.getCount());
    query.bind(3, log.getLenQuarters());
    query.bind(4, log.getDiameterQuarters());
    query.bind(5, log.getCostCentsQuarters());
    query.bind(6, log.getQuality());
    query.bind(7, log.getLocation());
    query.bind(8, log.getNotes());
    query.exec();

    // Get the ID of the inserted log, and update the log
    log.setID(this->db.getLastInsertRowid());
    cout << "DATABASE: Log does not exist, inserted with ID " << log.getId() << endl;
    log.update();

    return log;
}