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
        query.getColumn(6).getText()
    );
}