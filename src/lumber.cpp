#include "lumber.hpp"
#include "interfaces.hpp"
#include "types.hpp"
#include "logs.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include "wwhg_datamodel.hpp"
#include <QSqlQuery>
#include <QBuffer>
#include <QPixmap>
#include <QVariant>

// DDL
// CREATE TABLE lumber (
//     id                 INTEGER NOT NULL
//                                UNIQUE
//                                PRIMARY KEY AUTOINCREMENT,
//     from_log           INTEGER REFERENCES logs (id)
//                                NOT NULL,
//     species            TEXT    NOT NULL,
//     len_quarters       INTEGER NOT NULL
//                                CHECK ( (len_quarters > 0) ),
//     width_quarters     INTEGER NOT NULL
//                                CHECK ( (width_quarters > 0) ),
//     thickness_quarters INTEGER NOT NULL
//                                CHECK ( (thickness_quarters > 0) ),
//     drying             INTEGER NOT NULL
//                                DEFAULT Wet,
//     surfacing          INTEGER NOT NULL
//                                CHECK ( (drying BETWEEN 0 AND 4) )
//                                DEFAULT (0),
//     location           TEXT    REFERENCES storage_bins (name),
//     notes              TEXT,
//     media              BLOB,
//     cut                INTEGER REFERENCES partial_cuts (id)
//                                NOT NULL
// );

namespace {
    constexpr const char* INSERT_SQL =
        "INSERT INTO lumber (species, thickness_quarters, len_quarters, width_quarters, drying, surfacing, location, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    constexpr const char* UPDATE_SQL =
        "UPDATE lumber SET species = ?, thickness_quarters = ?, len_quarters = ?, width_quarters = ?, drying = ?, "
        "surfacing = ?, location = ?, notes = ? WHERE id = ?;";
    constexpr const char* SELECT_ONE_SQL =
        "SELECT id, species, thickness_quarters, len_quarters, width_quarters, drying, surfacing, location, notes "
        "FROM lumber WHERE id = ?;";
    constexpr const char* SELECT_ALL_SQL =
        "SELECT id, species, thickness_quarters, len_quarters, width_quarters, drying, surfacing, location, notes "
        "FROM lumber;";
}

Lumber::Lumber(std::string species, unsigned thickness_eighths, unsigned len_quarters, unsigned width_eighths, Drying drying, bool smoothed, std::string location, std::string notes)
{
    id_ = -1;
    species_ = std::move(species);
    thickness_quarters_ = thickness_eighths * 4;
    len_quarters_ = len_quarters;
    width_quarters_ = width_eighths * 4;
    drying_ = drying;
    smoothed_ = smoothed;
    location_ = std::move(location);
    notes_ = std::move(notes);
}

bool Lumber::insert() {
    try {
        if (LUMBER_LOGGING)
            std::cout << "[Lumber] inserting — species: " << species_ << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            INSERT_SQL};

        stmt.bind(1, species_);
        stmt.bind(2, thickness_quarters_);
        stmt.bind(3, len_quarters_);
        stmt.bind(4, width_quarters_);
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

bool Lumber::update() {
    try {
        if (LUMBER_LOGGING)
            std::cout << "[Lumber] updating — id: " << id_ << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            UPDATE_SQL};

        stmt.bind(1, species_);
        stmt.bind(2, thickness_quarters_);
        stmt.bind(3, len_quarters_);
        stmt.bind(4, width_quarters_);
        stmt.bind(5, static_cast<int>(drying_));
        stmt.bind(6, smoothed_ ? 1 : 0);
        stmt.bind(7, location_);
        stmt.bind(8, notes_);
        stmt.bind(9, id_);

        return (stmt.exec() > 0);
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (update): " << e.what() << "\n";
    }
    return false;
}

std::optional<Lumber> Lumber::get_by_id(int id) {
    try {
        if (LUMBER_LOGGING)
            std::cout << "[Lumber] getting by id — id: " << id << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            SELECT_ONE_SQL};

        stmt.bind(1, id);

        if (stmt.executeStep()) {
            Lumber lumber;
            lumber.id_ = stmt.getColumn(0).getInt();
            lumber.species_ = stmt.getColumn(1).getText();
            lumber.thickness_quarters_ = stmt.getColumn(2).getInt();
            lumber.len_quarters_ = stmt.getColumn(3).getInt();
            lumber.width_quarters_ = stmt.getColumn(4).getInt();
            lumber.drying_ = static_cast<Drying>(stmt.getColumn(5).getInt());
            lumber.smoothed_ = stmt.getColumn(6).getInt() != 0;
            lumber.location_ = stmt.getColumn(7).getText();
            lumber.notes_ = stmt.getColumn(8).getText();

            return lumber;
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (get_by_id): " << e.what() << "\n";
    }
    return std::nullopt;
}

std::vector<Lumber> Lumber::get_all() {
    std::vector<Lumber> lumberList;
    try {
        if (LUMBER_LOGGING)
            std::cout << "[Lumber] getting all" << std::endl;

        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE};
        SQLite::Statement stmt{db,
            SELECT_ALL_SQL};

        while (stmt.executeStep()) {
            Lumber lumber;
            lumber.id_ = stmt.getColumn(0).getInt();
            lumber.species_ = stmt.getColumn(1).getText();
            lumber.thickness_quarters_ = stmt.getColumn(2).getInt();
            lumber.len_quarters_ = stmt.getColumn(3).getInt();
            lumber.width_quarters_ = stmt.getColumn(4).getInt();
            lumber.drying_ = static_cast<Drying>(stmt.getColumn(5).getInt());
            lumber.smoothed_ = stmt.getColumn(6).getInt() != 0;
            lumber.location_ = stmt.getColumn(7).getText();
            lumber.notes_ = stmt.getColumn(8).getText();

            lumberList.push_back(lumber);
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite error (get_all): " << e.what() << "\n";
    }
    return lumberList;
}

// Converter to WWHG datamodel
wwhg::WwhgBoard Lumber::toWwhg() {
    double length_ft = len_quarters_ / 4.0 / 12.0;
    std::string size = std::to_string(thickness_quarters_ / 4.0) + "x" + std::to_string(width_quarters_ / 4.0);
    return wwhg::WwhgBoard(id_, species_, size, static_cast<unsigned>(length_ft), wwhg::WwhgSurfacing::RGH, 0.0);
}

QPixmap Lumber::loadPixmap() const {
    QSqlQuery query;
    query.prepare("SELECT media FROM lumber WHERE id = :id");
    query.bindValue(":id", QVariant(get_id()));
    if (!query.exec() || !query.next())
        return QPixmap();
    QByteArray blob = query.value(0).toByteArray();
    QPixmap pix;
    pix.loadFromData(blob, "JPEG");
    return pix;
}

bool Lumber::savePixmap(const QPixmap& pixmap) const {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "JPEG");
    QSqlQuery query;
    query.prepare("UPDATE lumber SET media = :media WHERE id = :id");
    query.bindValue(":media", QVariant(ba));
    query.bindValue(":id", get_id());
    return query.exec();
}