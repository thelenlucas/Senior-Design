#include "live_edge_slabs.hpp"
#include "db.hpp"

#include <string>
#include <stdexcept>

Slab::Slab(int id,
    std::string species,
    uint thickness_quarters,
    uint len_quarters,
    Drying drying,
    bool smoothed,
    std::string location,
    std::string notes,
    std::optional<Database*> db
) {
    this->id = id;
    this->species = species;
    this->thickness_quarters = thickness_quarters;
    this->len_quarters = len_quarters;
    this->drying = drying;
    this->smoothed = smoothed;
    this->location = location;
    this->notes = notes;
    this->db = db;
}

std::vector<Slab> Slab::allSlabs(Database *db) {
    if (!db) {
        throw std::runtime_error("Database is not connected");
    }

    SQLite::Database *database = db->getDb();
    SQLite::Statement query(*database, "SELECT * FROM live_edge_slabs");
    std::vector<Slab> slabs;

    while (query.executeStep()) {
        slabs.push_back(
            Slab(
                query.getColumn(0),
                query.getColumn(1),
                query.getColumn(2),
                query.getColumn(3),
                static_cast<Drying>(static_cast<int>(query.getColumn(4))),
                query.getColumn(5).getInt() != 0,
                query.getColumn(6),
                query.getColumn(7),
                db
            )
        );
    }

    return slabs;
}

void Slab::connect(Database *db) {
    this->db = std::optional<Database *>(db);
}

void Slab::remove() {
    if (!this->isConnected()) {
        throw std::runtime_error("Slab is not connected to a database");
    }

    SQLite::Database *db = this->db.value()->getDb();
    SQLite::Statement query(*db, "DELETE FROM live_edge_slabs WHERE id = ?");
    query.bind(1, this->id);
    query.exec();
}

void Slab::update() {
    if (!this->isConnected()) {
        throw std::runtime_error("Slab is not connected to a database");
    }

    SQLite::Database *db = this->db.value()->getDb();
    SQLite::Statement query(*db, "UPDATE live_edge_slabs SET species = ?, thickness_quarters = ?, len_quarters = ?, drying = ?, smoothed = ?, location = ?, notes = ? WHERE id = ?");
    query.bind(1, this->species);
    query.bind(2, this->thickness_quarters);
    query.bind(3, this->len_quarters);
    query.bind(4, static_cast<int>(this->drying));
    query.bind(5, this->smoothed);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    query.bind(8, this->id);
    query.exec();
}