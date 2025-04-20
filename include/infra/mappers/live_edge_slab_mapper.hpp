#pragma once
#include "domain/live_edge_slab.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain {
    inline QString LiveEdgeSlab::createDbSQL() {
        return u8R"(
            CREATE TABLE IF NOT EXISTS live_edge_slabs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                species TEXT NOT NULL,
                length REAL NOT NULL,
                width REAL NOT NULL,
                thickness REAL NOT NULL,
                drying INTEGER NOT NULL,
                surfacing INTEGER NOT NULL,
                location TEXT,
                notes TEXT
            )
        )";
    }

    inline QString LiveEdgeSlab::insertSQL()
    {
        return "INSERT INTO live_edge_slabs (species, length, width, thickness, drying, surfacing, location, notes) VALUES (:species, :length, :width, :thickness, :drying, :surfacing, :location, :notes)";
    }

    inline QString LiveEdgeSlab::updateSQL()
    {
        return "UPDATE live_edge_slabs SET species = :species, length = :length, width = :width, thickness = :thickness, drying = :drying, surfacing = :surfacing, location = :location, notes = :notes WHERE id = :id";
    }

    inline QString LiveEdgeSlab::selectOneSQL() { return u8R"(SELECT * FROM live_edge_slabs WHERE id=:id)"; }
    inline QString LiveEdgeSlab::selectAllSQL() { return u8R"(SELECT * FROM live_edge_slabs)"; }

    inline void LiveEdgeSlab::bindForInsert(QSqlQuery& q, const LiveEdgeSlab& slab)
    {
        q.bindValue(":species", QString::fromStdString(slab.species.name));
        q.bindValue(":length", slab.length.toInches());
        q.bindValue(":width", slab.width.toInches());
        q.bindValue(":thickness", slab.thickness.toInches());
        q.bindValue(":drying", static_cast<int>(slab.drying));
        q.bindValue(":surfacing", static_cast<int>(slab.surfacing));
        q.bindValue(":location", QString::fromStdString(slab.location));
        q.bindValue(":notes", QString::fromStdString(slab.notes));
    }

    inline void LiveEdgeSlab::bindForUpdate(QSqlQuery& q, const LiveEdgeSlab& slab)
    {
        q.bindValue(":species", QString::fromStdString(slab.species.name));
        q.bindValue(":length", slab.length.toInches());
        q.bindValue(":width", slab.width.toInches());
        q.bindValue(":thickness", slab.thickness.toInches());
        q.bindValue(":drying", static_cast<int>(slab.drying));
        q.bindValue(":surfacing", static_cast<int>(slab.surfacing));
        q.bindValue(":location", QString::fromStdString(slab.location));
        q.bindValue(":notes", QString::fromStdString(slab.notes));
        q.bindValue(":id", slab.id.id);
    }

    inline LiveEdgeSlab LiveEdgeSlab::fromRecord(const QSqlRecord& record)
    {
        return {
            .id = {record.value("id").toInt()},
            .species = {record.value("species").toString().toStdString()},
            .length = Length::fromInches(record.value("length").toDouble()),
            .width = Length::fromInches(record.value("width").toDouble()),
            .thickness = Length::fromInches(record.value("thickness").toDouble()),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .surfacing = static_cast<SlabSurfacing>(record.value("surfacing").toInt()),
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString()
        };
    }
}