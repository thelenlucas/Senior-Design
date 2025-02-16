#include "cookies.hpp"
#include "db.hpp"

#include <string>
#include <stdexcept>

Cookie::Cookie(
    int id,
    std::string species,
    uint thickness_quarters,
    uint diameter_quarters,
    Drying drying,
    std::string location,
    std::string notes,
    std::optional<Database*> db
) {
    this->id = id;
    this->species = species;
    this->thickness_quarters = thickness_quarters;
    this->diameter_quarters = diameter_quarters;
    this->drying = drying;
    this->location = location;
    this->notes = notes;
    this->db = db;
}

void Cookie::connect(Database* db) {
    this->db = optional<Database*>(db);
}

void Cookie::remove() {
    if (!this->isConnected()) {
        throw std::runtime_error("Cookie is not connected to a database");
    }

    SQLite::Database* db = this->db.value()->getDb();

    SQLite::Statement query(*db, "DELETE FROM cookies WHERE id = ?");
    query.bind(1, this->id);
    query.exec();
}

void Cookie::update() {
    if (!this->isConnected()) {
        throw std::runtime_error("Cookie is not connected to a database");
    }

    SQLite::Database* db = this->db.value()->getDb();

    SQLite::Statement query(*db, "UPDATE cookies SET species = ?, thickness_quarters = ?, diameter_quarters = ?, drying = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, this->species);
    query.bind(2, this->thickness_quarters);
    query.bind(3, this->diameter_quarters);
    query.bind(4, this->drying);
    query.bind(5, this->location);
    query.bind(6, this->notes);
    query.bind(7, this->id);
    query.exec();
}