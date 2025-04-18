// ---------------------------------------------------------------------------------------------------------------------
//  logs.cpp – persistence & workflow logic for Log
// ---------------------------------------------------------------------------------------------------------------------

#include "logs.hpp"
#include "wwhg_datamodel.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std::string_literals;

namespace
{
    constexpr bool kEnableLogging = LOGS_LOGGING;
    constexpr const char* kDbFile = DATABASE_FILE; // defined by build system
}

// ---------------------------------------------------------------------------------------------------------------------
//  Construction
// ---------------------------------------------------------------------------------------------------------------------

Log::Log(int                id,
         std::string        species,
         unsigned           len_quarters,
         unsigned           diameter_quarters,
         unsigned           cost_cents_quarters,
         unsigned           quality,
         Drying             drying,
         std::string        location,
         std::string        notes)
    : id_{id},
      species_{std::move(species)},
      len_quarters_{len_quarters},
      diameter_quarters_{diameter_quarters},
      cost_cents_quarters_{cost_cents_quarters},
      quality_{quality},
      location_{std::move(location)},
      notes_{std::move(notes)},
      drying_{drying} {}

// ---------------------------------------------------------------------------------------------------------------------
//  Helper – available length
// ---------------------------------------------------------------------------------------------------------------------

unsigned Log::getAvailableLength() const
{
    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db,
            "SELECT SUM(len_quarters) FROM taken_len_all WHERE from_log = ?"};
        stmt.bind(1, id_);

        unsigned taken_len = 0;
        if (stmt.executeStep() && !stmt.getColumn(0).isNull())
            taken_len = static_cast<unsigned>(stmt.getColumn(0).getInt());

        return (len_quarters_ >= taken_len) ? (len_quarters_ - taken_len) : 0U;
    }
    catch (const std::exception& e) {
        if (kEnableLogging)
            std::cerr << "Log::getAvailableLength() failed — " << e.what() << std::endl;
        return 0;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
//  Persistent interface – database helpers
// ---------------------------------------------------------------------------------------------------------------------

bool Log::insert()
{
    try {
        if (kEnableLogging)
            std::cout << "[Log] inserting — species: " << species_ << std::endl;

        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "INSERT INTO logs (species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes, drying) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"};

        stmt.bind(1, species_);
        stmt.bind(2, static_cast<int>(len_quarters_));
        stmt.bind(3, static_cast<int>(diameter_quarters_));
        stmt.bind(4, static_cast<int>(cost_cents_quarters_));
        stmt.bind(5, static_cast<int>(quality_));
        stmt.bind(6, location_);
        stmt.bind(7, notes_);
        stmt.bind(8, static_cast<int>(drying_));
        stmt.exec();

        id_ = static_cast<int>(db.getLastInsertRowid());
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Log::insert() failed — " << e.what() << std::endl;
        return false;
    }
}

bool Log::update()
{
    try {
        if (kEnableLogging)
            std::cout << "[Log] updating id=" << id_ << std::endl;

        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "UPDATE logs SET species = ?, len_quarters = ?, diameter_quarters = ?, cost_cents_quarters = ?, quality = ?, "
            "location = ?, notes = ?, drying = ? WHERE id = ?"};

        stmt.bind(1, species_);
        stmt.bind(2, static_cast<int>(len_quarters_));
        stmt.bind(3, static_cast<int>(diameter_quarters_));
        stmt.bind(4, static_cast<int>(cost_cents_quarters_));
        stmt.bind(5, static_cast<int>(quality_));
        stmt.bind(6, location_);
        stmt.bind(7, notes_);
        stmt.bind(8, static_cast<int>(drying_));
        stmt.bind(9, id_);
        stmt.exec();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Log::update() failed — " << e.what() << std::endl;
        return false;
    }
}

std::optional<Log> Log::get_by_id(int id)
{
    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db,
            "SELECT species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes, drying "
            "FROM logs WHERE id = ?"};
        stmt.bind(1, id);

        if (!stmt.executeStep())
            return std::nullopt;

        return Log{id,
                   stmt.getColumn(0).getString(),
                   static_cast<unsigned>(stmt.getColumn(1).getInt()),
                   static_cast<unsigned>(stmt.getColumn(2).getInt()),
                   static_cast<unsigned>(stmt.getColumn(3).getInt()),
                   static_cast<unsigned>(stmt.getColumn(4).getInt()),
                   static_cast<Drying>(stmt.getColumn(7).getInt()),
                   stmt.getColumn(5).getString(),
                   stmt.getColumn(6).getString()};
    }
    catch (const std::exception& e) {
        std::cerr << "Log::get_by_id() failed — " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::vector<Log> Log::get_all()
{
    std::vector<Log> result;

    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READONLY};
        SQLite::Statement stmt{db,
            "SELECT id, species, len_quarters, diameter_quarters, cost_cents_quarters, quality, location, notes, drying "
            "FROM logs WHERE scrapped = 0"};

        while (stmt.executeStep()) {
            result.emplace_back(
                stmt.getColumn(0).getInt(),
                stmt.getColumn(1).getString(),
                static_cast<unsigned>(stmt.getColumn(2).getInt()),
                static_cast<unsigned>(stmt.getColumn(3).getInt()),
                static_cast<unsigned>(stmt.getColumn(4).getInt()),
                static_cast<unsigned>(stmt.getColumn(5).getInt()),
                static_cast<Drying>(stmt.getColumn(8).getInt()),
                stmt.getColumn(6).getString(),
                stmt.getColumn(7).getString());
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Log::get_all() failed — " << e.what() << std::endl;
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Workflow helpers
// ---------------------------------------------------------------------------------------------------------------------

void Log::scrap()
{
    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db, "UPDATE logs SET scrapped = 1 WHERE id = ?"};
        stmt.bind(1, id_);
        stmt.exec();

        if (kEnableLogging)
            std::cout << "[Log] id=" << id_ << " scrapped" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Log::scrap() failed — " << e.what() << std::endl;
    }
}

unsigned Log::multiCut(unsigned amt_quarters, std::string type)
{
    if (amt_quarters > len_quarters_) {
        std::cerr << "Log::multiCut() failed — amount exceeds log length" << std::endl;
        return 0;
    }

    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "INSERT INTO partial_cuts (from_log, len_quarters, type) VALUES (?, ?, ?)"};

        stmt.bind(1, id_);
        stmt.bind(2, static_cast<int>(amt_quarters));
        stmt.bind(3, type);
        stmt.exec();

        unsigned cut_id = static_cast<unsigned>(db.getLastInsertRowid());

        if (kEnableLogging)
            std::cout << "[Log] multiCut() created cut id=" << cut_id << std::endl;

        return cut_id;
    }
    catch (const std::exception& e) {
        std::cerr << "Log::multiCut() failed — " << e.what() << std::endl;
        return 0;
    }
}


// CREATE TABLE kerf_cuts (
//     id                   INTEGER PRIMARY KEY AUTOINCREMENT,
//     taken_len_sixteenths INTEGER NOT NULL,
//     from_log             INTEGER REFERENCES logs (id) 
// );
void Log::wasteKerf(unsigned keft_width_16ths) {
    try {
        SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            "INSERT INTO kerf_cuts (from_log, taken_len_sixteenths) VALUES (?, ?)"};

        stmt.bind(1, id_);
        stmt.bind(2, static_cast<int>(keft_width_16ths));
        stmt.exec();

        if (kEnableLogging)
            std::cout << "[Log] wasteKerf() created kerf cut" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Log::wasteKerf() failed — " << e.what() << std::endl;
    }
}

// Converter to WWHG datamodel
wwhg::WwhgBoard Log::toWwhg() {
    double length_ft = getAvailableLength() / 4.0 / 12.0;
    std::string size = std::to_string(diameter_quarters_ / 4.0) + "in";
    return wwhg::WwhgBoard(id_, species_, size, static_cast<unsigned>(length_ft), wwhg::WwhgSurfacing::RGH, 0.0);
}

// - Deprecated. We calculate the length of logs via items cut from them now
// void Log::cut_length(unsigned amt_quarters)
// {
//     if (amt_quarters > len_quarters_)
//         amt_quarters = len_quarters_;

//     len_quarters_ -= amt_quarters;

//     try {
//         SQLite::Database db{kDbFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
//         SQLite::Statement stmt{db,
//             "UPDATE logs SET len_quarters = ? WHERE id = ?"};
//         stmt.bind(1, static_cast<int>(len_quarters_));
//         stmt.bind(2, id_);
//         stmt.exec();
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Log::cut_length() failed — " << e.what() << std::endl;
//     }
// }