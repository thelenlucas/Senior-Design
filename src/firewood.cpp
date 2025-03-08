#include "firewood.hpp"
#include <string>
#include <stdexcept>
#include "types.hpp"
#include "interfaces.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include "logs.hpp"
#include <math.h>
#include <iostream>
#include <iomanip>

// Schema:
// CREATE TABLE firewood (
//     id                 INTEGER PRIMARY KEY AUTOINCREMENT
//                                UNIQUE
//                                NOT NULL,
//     from_log           INTEGER REFERENCES logs (id) 
//                                NOT NULL,
//     species            TEXT    NOT NULL,
//     drying             INTEGER NOT NULL
//                                CHECK ( (drying BETWEEN 0 AND 3) ),
//     feet_3             INTEGER CHECK ( (feet_3 >= 0) ) 
//                                NOT NULL,
//     location           VARCHAR REFERENCES storage_bins (name),
//     notes              TEXT,
//     media              BLOB,
//     taken_len_quarters INTEGER NOT NULL
// );

/* Firewood C++ File:
 *  - Ties the Firewood characteristics to each object created
 * Includes the following functions: 
 *  - Insert
 *  - Update
 *  - Get-by-ID
 *  - Get All
 *  - Make From Log
*/
Firewood::Firewood(
    int id,
    std::string species,
    Drying drying,
    uint feet_3,
    uint taken_len_quarters,
    std::string location,
    std::string notes
) {
    this->id = id;
    this->species = species;
    this->drying = drying;
    this->feet_3 = feet_3;
    this->taken_len_quarters = taken_len_quarters;
    this->location = location;
    this->notes = notes;
}

// Insert the object in the database
bool Firewood::insert() {
    if (FIREWOOD_LOGGING) std::cout << "Inserting firewood: " << this->species << std::endl;
    SQLite::Database db("firewood.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    SQLite::Statement query(db, "INSERT INTO firewood (species, drying, feet_3, location, notes) VALUES (?, ?, ?, ?, ?)");
    query.bind(1, this->species);
    query.bind(2, this->drying);
    query.bind(3, this->feet_3);
    query.bind(4, this->location);
    query.bind(5, this->notes);
    query.exec();
    this->id = db.getLastInsertRowid();
    return true;
}

// Update the object in the database
bool Firewood::update() {
    if (FIREWOOD_LOGGING) std::cout << "Updating firewood: " << this->id << std::endl;
    SQLite::Database db("firewood.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    SQLite::Statement query(db, "UPDATE firewood SET species = ?, drying = ?, feet_3 = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, this->species);
    query.bind(2, this->drying);
    query.bind(3, this->feet_3);
    query.bind(4, this->location);
    query.bind(5, this->notes);
    query.bind(6, this->id);
    query.exec();
    return true;
}

// Get by ID
std::optional<Firewood> Firewood::get_by_id(int id) {
    if (FIREWOOD_LOGGING) std::cout << "Getting firewood by id: " << id << std::endl;
    SQLite::Database db("firewood.db", SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM firewood WHERE id = ?");
    query.bind(1, id);
    if (query.executeStep()) {
        return Firewood(
            query.getColumn(0).getInt(),
            query.getColumn(2).getText(),
            static_cast<Drying>(query.getColumn(3).getInt()),
            query.getColumn(4).getInt(),
            query.getColumn(8).getInt(),
            query.getColumn(5).getText(),
            query.getColumn(6).getText()
        );
    }
    return std::nullopt;
}

// Get all
std::vector<Firewood> Firewood::get_all() {
    SQLite::Database db("firewood.db", SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM firewood");
    std::vector<Firewood> firewood;
    while (query.executeStep()) {
        firewood.push_back(Firewood(
            query.getColumn(0).getInt(),
            query.getColumn(2).getText(),
            static_cast<Drying>(query.getColumn(3).getInt()),
            query.getColumn(4).getInt(),
            query.getColumn(8).getInt(),
            query.getColumn(5).getText(),
            query.getColumn(6).getText()
        ));
    }
    return firewood;
}

// Make from log
std::vector<Firewood> Firewood::make_from_log(
    Log log,
    int len_quarters,
    std::optional<int> thickness_quarters,
    std::optional<int> width_quarters,
    std::optional<Drying> drying
) {
    // The volume is calculated assuming the log is a cylinder
    float length_feet = len_quarters / 4.0 / 12.0;
    float diameter_feet = log.getDiameterQuarters() / 4.0 / 12.0;
    float cubic_feet = 3.14159 * diameter_feet * diameter_feet * length_feet / 4.0;
    int feet_3 = static_cast<int>(floor(cubic_feet));

    std::vector<Firewood> firewood;
    // This is the most simple cut that exists, we just return a single item
    firewood.push_back(Firewood(0, log.getSpecies(), drying.value_or(Drying::KILN_DRIED), length_feet, feet_3));
    return firewood;
}