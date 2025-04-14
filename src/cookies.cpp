// ---------------------------------------------------------------------------------------------------------------------
//  cookie.cpp – persistence & manufacturing logic for Cookie
// ---------------------------------------------------------------------------------------------------------------------

#include "cookies.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std::string_literals;

namespace
{
    constexpr bool kEnableLogging = COOKIES_LOGGING;
    constexpr const char* kDbFile = "cookies.db"; // adjust if you store all tables in a common DB
}

// ---------------------------------------------------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------------------------------------------------

Cookie::Cookie(int                id,
               int                from_log,
               std::string        species,
               unsigned           thickness_quarters,
               unsigned           diameter_quarters,
               Drying             drying,
               std::string        location,
               std::string        notes)
    : id_{id},
      from_log_{from_log},
      species_{std::move(species)},
      thickness_quarters_{thickness_quarters},
      diameter_quarters_{diameter_quarters},
      drying_{drying},
      location_{std::move(location)},
      notes_{std::move(notes)} {}

int Cookie::get_id() const noexcept { return id_; }

// ---------------------------------------------------------------------------------------------------------------------
//  Persistent interface – database helpers
// ---------------------------------------------------------------------------------------------------------------------

bool Cookie::insert()
{
    try {
        if (kEnableLogging)
            std::cout << "[Cookie] inserting — species: " << species_ << std::endl;

        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "INSERT INTO cookies (from_log, species, thickness_quarters, diameter_quarters, drying, location, notes) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)"};

        stmt.bind(1, from_log_);
        stmt.bind(2, species_);
        stmt.bind(3, static_cast<int>(thickness_quarters_));
        stmt.bind(4, static_cast<int>(diameter_quarters_));
        stmt.bind(5, static_cast<int>(drying_));
        stmt.bind(6, location_);
        stmt.bind(7, notes_);
        stmt.exec();

        id_ = static_cast<int>(db.getLastInsertRowid());
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Cookie::insert() failed — " << e.what() << std::endl;
        return false;
    }
}

bool Cookie::update()
{
    try {
        if (kEnableLogging)
            std::cout << "[Cookie] updating id=" << id_ << std::endl;

        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "UPDATE cookies SET from_log = ?, species = ?, thickness_quarters = ?, diameter_quarters = ?, drying = ?, "
            "location = ?, notes = ? WHERE id = ?"};

        stmt.bind(1, from_log_);
        stmt.bind(2, species_);
        stmt.bind(3, static_cast<int>(thickness_quarters_));
        stmt.bind(4, static_cast<int>(diameter_quarters_));
        stmt.bind(5, static_cast<int>(drying_));
        stmt.bind(6, location_);
        stmt.bind(7, notes_);
        stmt.bind(8, id_);
        stmt.exec();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Cookie::update() failed — " << e.what() << std::endl;
        return false;
    }
}

std::optional<Cookie> Cookie::get_by_id(int id)
{
    try {
        if (kEnableLogging)
            std::cout << "[Cookie] fetching id=" << id << std::endl;

        SQLite::Database db{kDbFile, SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db, "SELECT * FROM cookies WHERE id = ?"};
        stmt.bind(1, id);

        if (!stmt.executeStep())
            return std::nullopt;

        return Cookie{
            stmt.getColumn(0).getInt(),                      // id
            stmt.getColumn(1).getInt(),                      // from_log
            stmt.getColumn(2).getString(),                   // species
            static_cast<unsigned>(stmt.getColumn(3).getInt()),// thickness
            static_cast<unsigned>(stmt.getColumn(4).getInt()),// diameter
            static_cast<Drying>(stmt.getColumn(5).getInt()), // drying
            stmt.getColumn(6).getString(),                   // location
            stmt.getColumn(7).getString()};                  // notes
    }
    catch (const std::exception& e) {
        std::cerr << "Cookie::get_by_id() failed — " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<Cookie> Cookie::get_all()
{
    std::vector<Cookie> result;

    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db, "SELECT * FROM cookies"};

        while (stmt.executeStep()) {
            result.emplace_back(
                stmt.getColumn(0).getInt(),
                stmt.getColumn(1).getInt(),
                stmt.getColumn(2).getString(),
                static_cast<unsigned>(stmt.getColumn(3).getInt()),
                static_cast<unsigned>(stmt.getColumn(4).getInt()),
                static_cast<Drying>(stmt.getColumn(5).getInt()),
                stmt.getColumn(6).getString(),
                stmt.getColumn(7).getString());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Cookie::get_all() failed — " << e.what() << std::endl;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Manufacturable interface – derive Cookies from a Log
// ---------------------------------------------------------------------------------------------------------------------

std::vector<Cookie> Cookie::make_from_log(Log                     log,
                                          unsigned                thickness_quarters,
                                          std::optional<int>      diameter_quarters,
                                          std::optional<Drying>   drying)
{
    std::vector<Cookie> out;

    out.emplace_back(-1,                                         // id (to persist later)
                     log.get_id(),                               // from_log
                     log.getSpecies(),                           // species
                     thickness_quarters,                         // thickness
                     static_cast<unsigned>(
                         diameter_quarters.value_or(log.getDiameterQuarters())),
                     drying.value_or(Drying::KILN_DRIED),        // drying
                     /* location */ "", /* notes */ "");
    return out;
}
