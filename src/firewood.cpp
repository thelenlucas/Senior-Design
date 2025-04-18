// ---------------------------------------------------------------------------------------------------------------------
//  firewood.cpp – persistence & manufacturing logic for Firewood
// ---------------------------------------------------------------------------------------------------------------------

#include "firewood.hpp"
#include "wwhg_datamodel.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace std::string_literals; // convenience for string literals

namespace
{
    constexpr bool kEnableLogging = FIREWOOD_LOGGING;
    constexpr double kPi          = 3.14159265358979323846;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------------------------------------------------

Firewood::Firewood(int                id,
                   std::string        species,
                   Drying             drying,
                   unsigned           cubic_feet,
                   unsigned           taken_len_quarters,
                   std::string        location,
                   std::string        notes)
    : id_{id},
      species_{std::move(species)},
      drying_{drying},
      cubic_feet_{cubic_feet},
      taken_len_quarters_{taken_len_quarters},
      location_{std::move(location)},
      notes_{std::move(notes)} {}

int Firewood::get_id() const noexcept { return id_; }

// ---------------------------------------------------------------------------------------------------------------------
//  Persistent interface – database helpers
// ---------------------------------------------------------------------------------------------------------------------

bool Firewood::insert()
{
    try {
        if (kEnableLogging)
            std::cout << "[Firewood] inserting — species: " << species_ << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "INSERT INTO firewood (species, drying, feet_3, location, notes, taken_len_quarters) "
            "VALUES (?, ?, ?, ?, ?, ?)"};

        stmt.bind(1, species_);
        stmt.bind(2, static_cast<int>(drying_));
        stmt.bind(3, static_cast<int>(cubic_feet_));
        stmt.bind(4, location_);
        stmt.bind(5, notes_);
        stmt.bind(6, static_cast<int>(taken_len_quarters_));
        stmt.exec();

        id_ = static_cast<int>(db.getLastInsertRowid());
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Firewood::insert() failed — " << e.what() << std::endl;
        return false;
    }
}

bool Firewood::update()
{
    try {
        if (kEnableLogging)
            std::cout << "[Firewood] updating id=" << id_ << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "UPDATE firewood SET species = ?, drying = ?, feet_3 = ?, location = ?, notes = ?, "
            "taken_len_quarters = ? WHERE id = ?"};

        stmt.bind(1, species_);
        stmt.bind(2, static_cast<int>(drying_));
        stmt.bind(3, static_cast<int>(cubic_feet_));
        stmt.bind(4, location_);
        stmt.bind(5, notes_);
        stmt.bind(6, static_cast<int>(taken_len_quarters_));
        stmt.bind(7, id_);
        stmt.exec();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Firewood::update() failed — " << e.what() << std::endl;
        return false;
    }
}

std::optional<Firewood> Firewood::get_by_id(int id)
{
    try {
        if (kEnableLogging)
            std::cout << "[Firewood] fetching id=" << id << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db, "SELECT * FROM firewood WHERE id = ?"};
        stmt.bind(1, id);

        if (!stmt.executeStep())
            return std::nullopt;

        return Firewood{
            stmt.getColumn(0).getInt(),                      // id
            stmt.getColumn(2).getString(),                   // species
            static_cast<Drying>(stmt.getColumn(3).getInt()), // drying
            static_cast<unsigned>(stmt.getColumn(4).getInt()),// feet_3
            static_cast<unsigned>(stmt.getColumn(8).getInt()),// taken_len_quarters
            stmt.getColumn(5).getString(),                   // location
            stmt.getColumn(6).getString()};                  // notes
    }
    catch (const std::exception& e) {
        std::cerr << "Firewood::get_by_id() failed — " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<Firewood> Firewood::get_all()
{
    std::vector<Firewood> result;

    try {
        SQLite::Database db{"test.db", SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db, "SELECT * FROM firewood"};

        while (stmt.executeStep()) {
            result.emplace_back(
                stmt.getColumn(0).getInt(),
                stmt.getColumn(2).getString(),
                static_cast<Drying>(stmt.getColumn(3).getInt()),
                static_cast<unsigned>(stmt.getColumn(4).getInt()),
                static_cast<unsigned>(stmt.getColumn(8).getInt()),
                stmt.getColumn(5).getString(),
                stmt.getColumn(6).getString());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Firewood::get_all() failed — " << e.what() << std::endl;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Manufacturable interface – derive Firewood from a Log
// ---------------------------------------------------------------------------------------------------------------------

std::vector<Firewood> Firewood::make_from_log(Log                     log,
                                              int                     len_quarters,
                                              std::optional<int>      /* thickness_quarters unused */,
                                              std::optional<int>      /* width_quarters     unused */,
                                              std::optional<Drying>   drying)
{
    // Compute volume of a cylindrical log section (ft³)
    const double length_in   = len_quarters / 4.0;               // quarter‑in → inches
    const double diameter_in = log.getDiameterQuarters() / 4.0;  // quarter‑in → inches

    const double radius_ft   = (diameter_in / 2.0) / 12.0;       // inches → feet
    const double length_ft   = length_in / 12.0;                 // inches → feet

    const double volume_ft3  = kPi * radius_ft * radius_ft * length_ft;
    const unsigned cubic_ft  = static_cast<unsigned>(std::floor(volume_ft3));

    std::vector<Firewood> out;
    out.emplace_back(-1,                                    // id (to be persisted later)
                     log.getSpecies(),
                     drying.value_or(Drying::KILN_DRIED),
                     cubic_ft,
                     static_cast<unsigned>(len_quarters),
                     /* location */ "", /* notes */ "");
    return out;
}

// Converter to WWHG datamodel
wwhg::WwhgFirewoodBundle Firewood::toWwhg() {
    return wwhg::WwhgFirewoodBundle(id_, species_, static_cast<double>(cubic_feet_), /*moisture*/"", /*price*/0.0);
}
