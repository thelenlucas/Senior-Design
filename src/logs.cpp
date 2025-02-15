#include "logs.hpp"

#include <string>
#include <stdexcept>

Log::Log(int id,
        std::string species,
        uint len_quarters,
        uint diameter_quarters,
        uint cost_cents_quarters,
        uint quality,
        std::string location,
        std::string notes,
        std::optional<Database*> db
    ) {
        this->id = id;
        this->species = species;
        this->len_quarters = len_quarters;
        this->diameter_quarters = diameter_quarters;
        this->cost_cents_quarters = cost_cents_quarters;
        this->quality = quality;
        this->location = location;
        this->notes = notes;
        this->db = db;
    }

// Connects this log to a database
void Log::connect(Database* db) {
    this->db = optional<Database*>(db);
}

// Removes this item from the database, throws an error if this is disconnected
void Log::remove() {
    if (!this->isConnected()) {
        throw std::runtime_error("Log is not connected to a database");
    }

    this->db.value()->deleteLog(*this);
}

// Updates this item in the database, throws an error if this is disconnected
void Log::update() {
    if (!this->isConnected()) {
        throw std::runtime_error("Log is not connected to a database");
    }

    this->db.value()->updateLog(*this);
}
    