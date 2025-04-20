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
                worth INTEGER NOT NULL,
                location TEXT,
                notes TEXT
            )
        )";
    }

    inline QString LiveEdgeSlab::individualViewSQL()
    {
        return u8R"(CREATE VIEW IF NOT EXISTS display_slabs AS SELECT id AS 'ID', species AS 'Species', ROUND(length/16.0,2) AS 'Length (in)', ROUND(width/16.0,2) AS 'Width (in)', ROUND(thickness/16.0,2) AS 'Thickness (in)', CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying', CASE surfacing WHEN 0 THEN 'RGH' WHEN 1 THEN 'S1S' WHEN 2 THEN 'S2S' END AS 'Surfacing', printf('%.2f',worth/100.0) AS 'Worth ($)', location AS 'Location', notes AS 'Notes' FROM live_edge_slabs)";
    }

    inline QString LiveEdgeSlab::groupedViewSQL()
    {
        return "CREATE VIEW IF NOT EXISTS display_slabs_grouped AS SELECT COUNT(*) AS 'Count', species AS 'Species', ROUND(length/16.0,2) AS 'Length (in)', ROUND(width/16.0,2) AS 'Width (in)', ROUND(thickness/16.0,2) AS 'Thickness (in)', CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying', CASE surfacing WHEN 0 THEN 'RGH' WHEN 1 THEN 'S1S' WHEN 2 THEN 'S2S' END AS 'Surfacing', ROUND(AVG(worth)/100.0,2) AS 'Avg Worth ($)' FROM live_edge_slabs GROUP BY species, ROUND(length/16.0,2), ROUND(width/16.0,2), ROUND(thickness/16.0,2), drying, surfacing";
    }

    inline QString LiveEdgeSlab::insertSQL()
    {
        return "INSERT INTO live_edge_slabs (species, length, width, thickness, drying, surfacing, worth, location, notes) VALUES (:species, :length, :width, :thickness, :drying, :surfacing, :worth, :location, :notes)";
    }

    inline QString LiveEdgeSlab::updateSQL()
    {
        return "UPDATE live_edge_slabs SET species = :species, length = :length, width = :width, thickness = :thickness, drying = :drying, surfacing = :surfacing, worth = :worth, location = :location, notes = :notes WHERE id = :id";
    }

    inline QString LiveEdgeSlab::selectOneSQL() { return u8R"(SELECT * FROM live_edge_slabs WHERE id=:id)"; }
    inline QString LiveEdgeSlab::selectAllSQL() { return u8R"(SELECT * FROM live_edge_slabs)"; }

    // Add delete SQL
    inline QString LiveEdgeSlab::deleteSQL() { return u8R"(DELETE FROM live_edge_slabs WHERE id=:id)"; }

    inline void LiveEdgeSlab::bindForInsert(QSqlQuery& q, const LiveEdgeSlab& slab)
    {
        q.bindValue(":species", QString::fromStdString(slab.species.name));
        q.bindValue(":length", slab.length.toTicks());
        q.bindValue(":width", slab.width.toTicks());
        q.bindValue(":thickness", slab.thickness.toTicks());
        q.bindValue(":drying", static_cast<int>(slab.drying));
        q.bindValue(":surfacing", static_cast<int>(slab.surfacing));
        q.bindValue(":worth", static_cast<int>(slab.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(slab.location));
        q.bindValue(":notes", QString::fromStdString(slab.notes));
    }

    inline void LiveEdgeSlab::bindForUpdate(QSqlQuery& q, const LiveEdgeSlab& slab)
    {
        q.bindValue(":species", QString::fromStdString(slab.species.name));
        q.bindValue(":length", slab.length.toTicks());
        q.bindValue(":width", slab.width.toTicks());
        q.bindValue(":thickness", slab.thickness.toTicks());
        q.bindValue(":drying", static_cast<int>(slab.drying));
        q.bindValue(":surfacing", static_cast<int>(slab.surfacing));
        q.bindValue(":worth", static_cast<int>(slab.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(slab.location));
        q.bindValue(":notes", QString::fromStdString(slab.notes));
        q.bindValue(":id", slab.id.id);
    }

    inline LiveEdgeSlab LiveEdgeSlab::fromRecord(const QSqlRecord& record)
    {
        return {
            .id = {record.value("id").toInt()},
            .species = {record.value("species").toString().toStdString()},
            .length = Length::fromTicks(record.value("length").toDouble()),
            .width = Length::fromTicks(record.value("width").toDouble()),
            .thickness = Length::fromTicks(record.value("thickness").toDouble()),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .surfacing = static_cast<SlabSurfacing>(record.value("surfacing").toInt()),
            .worth = { record.value("worth").toInt() },
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString()
        };
    }
}