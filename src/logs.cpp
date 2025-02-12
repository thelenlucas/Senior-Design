#include "logs.hpp"

#include <string>
#include <stdexcept>

using std::cout, std::endl, std::string, std::runtime_error, std::optional;

Log::Log(int id,
        string species,
        std::uint32_t count,
        std::uint32_t len_quarters,
        std::uint32_t diameter_quarters,
        std::uint32_t cost_cents_quarters,
        std::uint32_t quality,
        string location,
        string notes,
        optional<Database*> db
    ) {
        this->id = id;
        this->species = species;
        this->count = count;
        this->len_quarters = len_quarters;
        this->diameter_quarters = diameter_quarters;
        this->cost_cents_quarters = cost_cents_quarters;
        this->quality = quality;
        this->location = location;
        this->notes = notes;
        this->db = db;
    }

void Log::combine(Log other) {
    if (LOGS_LOGGING) {
        if (other.isConnected()) {cout << "LOGS: Log " << this->id << " combining with log " << other.getId() << endl; }
        else { cout << "LOGS: Log " << this->id << " combining with disconnected log (presumed new)"; }
    }

    if (!this->isConnected()) {
        throw runtime_error("Primary log is not connected to a database");
    }

    // Up the count
    this->count += other.getCount();

    // Average our cost per quarter
    this->cost_cents_quarters = (this->cost_cents_quarters + other.getCostCentsQuarters()) / 2;

    // Update this log in the database
    this->update();

    // If the other log already exists, remove it
    if (other.isConnected()) {
        other.remove();
    }
}

// Connects this log to a database
void Log::connect(Database* db) {
    this->db = optional<Database*>(db);
}

// Removes this item from the database, throws an error if this is disconnected
void Log::remove() {
    if (!this->isConnected()) {
        throw runtime_error("Log is not connected to a database");
    }

    this->db.value()->deleteLog(*this);
}

// Updates this item in the database, throws an error if this is disconnected
void Log::update() {
    if (!this->isConnected()) {
        throw runtime_error("Log is not connected to a database");
    }

    this->db.value()->updateLog(*this);
}
    