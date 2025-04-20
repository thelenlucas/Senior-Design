#pragma once
#include "domain/lumber.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain {
    inline QString Lumber::createDbSQL() {
        return u8R"(
            CREATE TABLE IF NOT EXISTS lumber (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                species TEXT NOT NULL,
                length REAL NOT NULL,
                width REAL NOT NULL,
                thickness REAL NOT NULL,
                drying INTEGER NOT NULL,
                surfacing INTEGER NOT NULL,
                worth INTEGER NOT NULL,
                location TEXT,
                notes TEXT
            )
        )";
    }

    inline QString Lumber::insertSQL() {
        return "INSERT INTO lumber (species, length, width, thickness, drying, surfacing, worth, location, notes) VALUES (:species, :length, :width, :thickness, :drying, :surfacing, :worth, :location, :notes)";
    }

    inline QString Lumber::updateSQL() {
        return "UPDATE lumber SET species = :species, length = :length, width = :width, thickness = :thickness, drying = :drying, surfacing = :surfacing, worth = :worth, location = :location, notes = :notes WHERE id = :id";
    }

    inline QString Lumber::selectOneSQL() { return u8R"(SELECT * FROM lumber WHERE id=:id)"; }
    inline QString Lumber::selectAllSQL() { return u8R"(SELECT * FROM lumber)"; }

    inline void Lumber::bindForInsert(QSqlQuery& q, const Lumber& l) {
        q.bindValue(":species", QString::fromStdString(l.species.name));
        q.bindValue(":length", l.length.toInches());
        q.bindValue(":width", l.width.toInches());
        q.bindValue(":thickness", l.thickness.toInches());
        q.bindValue(":drying", static_cast<int>(l.drying));
        q.bindValue(":surfacing", static_cast<int>(l.surfacing));
        q.bindValue(":worth", static_cast<int>(l.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(l.location));
        q.bindValue(":notes", QString::fromStdString(l.notes));
    }

    inline void Lumber::bindForUpdate(QSqlQuery& q, const Lumber& l) {
        q.bindValue(":species", QString::fromStdString(l.species.name));
        q.bindValue(":length", l.length.toInches());
        q.bindValue(":width", l.width.toInches());
        q.bindValue(":thickness", l.thickness.toInches());
        q.bindValue(":drying", static_cast<int>(l.drying));
        q.bindValue(":surfacing", static_cast<int>(l.surfacing));
        q.bindValue(":worth", static_cast<int>(l.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(l.location));
        q.bindValue(":notes", QString::fromStdString(l.notes));
        q.bindValue(":id", l.id.id);
    }

    inline Lumber Lumber::fromRecord(const QSqlRecord& record) {
        return {
            .id = { record.value("id").toInt() },
            .species = { record.value("species").toString().toStdString() },
            .length = Length::fromInches(record.value("length").toDouble()),
            .width = Length::fromInches(record.value("width").toDouble()),
            .thickness = Length::fromInches(record.value("thickness").toDouble()),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .surfacing = static_cast<LumberSurfacing>(record.value("surfacing").toInt()),
            .worth = { record.value("worth").toInt() },
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString()
        };
    }
}