#include "firewood.hpp"
#include "db.hpp"

#include <string>
#include <stdexcept>

Firewood::Firewood(
    int id,
    std::string species,
    Drying drying,
    uint feet_3,
    std::string location,
    std::string notes,
    std::optional<Database *> db
) {
    this->id = id;
    this->species = species;
    this->drying = drying;
    this->feet_3 = feet_3;
    this->location = location;
    this->notes = notes;
    this->db = db;
}

std::vector<Firewood> Firewood::allFirewood(Database *db) {
    if (!db) {
        throw std::runtime_error("Database is not connected");
    }

    SQLite::Database *database = db->getDb();
    SQLite::Statement query(*database, "SELECT * FROM firewood");
    std::vector<Firewood> firewood;

    while (query.executeStep()) {
        firewood.push_back(
            Firewood(
                query.getColumn(0),
                query.getColumn(1),
                static_cast<Drying>(static_cast<int>(query.getColumn(2))),
                query.getColumn(3),
                query.getColumn(4),
                query.getColumn(5),
                db
            )
        );
    }

    return firewood;
}

void Firewood::connect(Database *db) {
    this->db = std::optional<Database *>(db);
}

void Firewood::remove() {
    if (!this->isConnected()) {
        throw std::runtime_error("Firewood is not connected to a database");
    }

    SQLite::Database *db = this->db.value()->getDb();

    SQLite::Statement query(*db, "DELETE FROM firewood WHERE id = ?");
    query.bind(1, this->id);
    query.exec();
}

void Firewood::update() {
    if (!this->isConnected()) {
        throw std::runtime_error("Firewood is not connected to a database");
    }

    SQLite::Database *db = this->db.value()->getDb();

    SQLite::Statement query(*db, "UPDATE firewood SET species = ?, drying = ?, feet_3 = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, this->species);
    query.bind(2, static_cast<int>(this->drying));
    query.bind(3, this->feet_3);
    query.bind(4, this->location);
    query.bind(5, this->notes);
    query.bind(6, this->id);
    query.exec();
}