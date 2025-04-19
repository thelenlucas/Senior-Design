#include "live_edge_slabs.hpp"
#include "slab_manufacturer.hpp"
#include "types.hpp"          // DATABASE_FILE, Drying enum
#include "logs.hpp"            // Log class (for SlabManufacturer::finalize)
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <optional>
#include "wwhg_datamodel.hpp"
#include <QSqlQuery>
#include <QBuffer>
#include <QPixmap>
#include <QVariant>

// ---------------------------------------------------------------------------------------------------------------------
//  Slab – ctor & simple getters
// ---------------------------------------------------------------------------------------------------------------------

Slab::Slab(int                id,
           std::string        species,
           unsigned           thickness_eighths,
           unsigned           len_quarters,
           unsigned           width_eighths,
           Drying             drying,
           bool               smoothed,
           std::string        location,
           std::string        notes)
    : id_{id},
      species_{std::move(species)},
      thickness_eighths_{thickness_eighths},
      len_quarters_{len_quarters},
      width_eighths_{width_eighths},
      drying_{drying},
      smoothed_{smoothed},
      location_{std::move(location)},
      notes_{std::move(notes)} {}

int Slab::get_id() const noexcept { return id_; }

// Converter to WWHG datamodel
wwhg::WwhgSlab Slab::toWwhg() const {
    double width_in = width_eighths_ / 8.0;
    unsigned length_ft = static_cast<unsigned>(len_quarters_ / 4.0 / 12.0);
    double thickness_in = thickness_eighths_ / 8.0;
    return wwhg::WwhgSlab(id_, species_, width_in, length_ft, thickness_in, wwhg::WwhgFinish::RGH, 0.0);
}

// ---------------------------------------------------------------------------------------------------------------------
//  Persistent interface – SQLite helpers
// ---------------------------------------------------------------------------------------------------------------------

namespace {
constexpr const char* INSERT_SQL =
    "INSERT INTO slabs (species, thickness_eighths, len_quarters, width_eighths, drying, smoothed, location, notes) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

constexpr const char* UPDATE_SQL =
    "UPDATE slabs SET species = ?, thickness_eighths = ?, len_quarters = ?, width_eighths = ?, drying = ?, "
    "smoothed = ?, location = ?, notes = ? WHERE id = ?;";

constexpr const char* SELECT_ONE_SQL =
    "SELECT id, species, thickness_eighths, len_quarters, width_eighths, drying, smoothed, location, notes "
    "FROM slabs WHERE id = ?;";

constexpr const char* SELECT_ALL_SQL =
    "SELECT id, species, thickness_eighths, len_quarters, width_eighths, drying, smoothed, location, notes FROM slabs;";
} // anonymous namespace

bool Slab::insert()
{
    if (id_ >= 0) {
        std::cerr << "Slab::insert() called on already‑persisted object (id=" << id_ << ")\n";
        return false;
    }

    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        SQLite::Statement stmt(db, INSERT_SQL);
        stmt.bind(1, species_);
        stmt.bind(2, thickness_eighths_);
        stmt.bind(3, len_quarters_);
        stmt.bind(4, width_eighths_);
        stmt.bind(5, static_cast<int>(drying_));
        stmt.bind(6, smoothed_ ? 1 : 0);
        stmt.bind(7, location_);
        stmt.bind(8, notes_);

        if (stmt.exec() > 0) {
            id_ = static_cast<int>(db.getLastInsertRowid());
            return true;
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (insert): " << e.what() << "\n";
    }
    return false;
}

bool Slab::update()
{
    if (id_ < 0) {
        std::cerr << "Slab::update() called on non‑persisted object (id=" << id_ << ")\n";
        return false;
    }

    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
        SQLite::Statement stmt(db, UPDATE_SQL);
        stmt.bind(1, species_);
        stmt.bind(2, thickness_eighths_);
        stmt.bind(3, len_quarters_);
        stmt.bind(4, width_eighths_);
        stmt.bind(5, static_cast<int>(drying_));
        stmt.bind(6, smoothed_ ? 1 : 0);
        stmt.bind(7, location_);
        stmt.bind(8, notes_);
        stmt.bind(9, id_);
        return stmt.exec() > 0;
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (update): " << e.what() << "\n";
    }
    return false;
}

std::optional<Slab> Slab::get_by_id(int id)
{
    if (id < 0) return std::nullopt;

    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        SQLite::Statement stmt(db, SELECT_ONE_SQL);
        stmt.bind(1, id);
        if (stmt.executeStep()) {
            return Slab(stmt.getColumn(0).getInt(),
                        stmt.getColumn(1).getText(),
                        stmt.getColumn(2).getUInt(),
                        stmt.getColumn(3).getUInt(),
                        stmt.getColumn(4).getUInt(),
                        static_cast<Drying>(stmt.getColumn(5).getInt()),
                        stmt.getColumn(6).getInt() == 1,
                        stmt.getColumn(7).getText(),
                        stmt.getColumn(8).getText());
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (get_by_id): " << e.what() << "\n";
    }
    return std::nullopt;
}

std::vector<Slab> Slab::get_all()
{
    std::vector<Slab> out;
    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        SQLite::Statement stmt(db, SELECT_ALL_SQL);
        while (stmt.executeStep()) {
            out.emplace_back(stmt.getColumn(0).getInt(),
                             stmt.getColumn(1).getText(),
                             stmt.getColumn(2).getUInt(),
                             stmt.getColumn(3).getUInt(),
                             stmt.getColumn(4).getUInt(),
                             static_cast<Drying>(stmt.getColumn(5).getInt()),
                             stmt.getColumn(6).getInt() == 1,
                             stmt.getColumn(7).getText(),
                             stmt.getColumn(8).getText());
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (get_all): " << e.what() << "\n";
    }
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------
//  Manufacturable interface helpers
// ---------------------------------------------------------------------------------------------------------------------

std::vector<Slab> Slab::make_from_log(Log                     log,
                                      int                     len_quarters,
                                      std::optional<int>      thickness_eighths,
                                      std::optional<int>      width_eighths,
                                      std::optional<Drying>   drying)
{
    Slab slab{-1,
              log.getSpecies(),
              static_cast<unsigned>(*thickness_eighths),
              static_cast<unsigned>(len_quarters),
              width_eighths.value(),
              drying.value_or(log.getDrying()),
              false,
              {},
              {}};

    slab.insert();


    return {std::move(slab)};
}

QPixmap Slab::loadPixmap() const {
    QSqlQuery query;
    query.prepare("SELECT media FROM slabs WHERE id = :id");
    query.bindValue(":id", get_id());
    if (!query.exec() || !query.next())
        return QPixmap();
    QByteArray blob = query.value(0).toByteArray();
    QPixmap pix;
    pix.loadFromData(blob, "JPEG");
    return pix;
}

bool Slab::savePixmap(const QPixmap& pixmap) const {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "JPEG");
    QSqlQuery query;
    query.bindValue(":media", QVariant::fromValue(ba));
    query.bindValue(":media", QVariant(ba));
    query.bindValue(":id", get_id());
    return query.exec();
}