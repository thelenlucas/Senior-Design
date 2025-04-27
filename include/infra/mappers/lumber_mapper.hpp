#pragma once
#include "domain/lumber.hpp"
#include "view_helpers.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain
{
    inline QString Lumber::createDbSQL()
    {
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
                notes TEXT,
                image BLOB
            )
        )";
    }

    inline QString Lumber::individualViewSQL()
    {
        return woodworks::infra::makeIndividualViewSQL(
            "display_lumber", "lumber",
            QStringList{
                "id AS 'ID'",
                "species AS 'Species'",
                "ROUND(length/16.0) AS 'Length (in)'",
                "printf('%d/4', thickness/4) AS 'Thickness'",
                "ROUND(width/16.0) AS 'Width (in)'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "CASE surfacing WHEN 0 THEN 'RGH' WHEN 1 THEN 'S1S' WHEN 2 THEN 'S2S' WHEN 3 THEN 'S3S' WHEN 4 THEN 'S4S' END AS 'Surfacing'",
                "printf('%.2f',worth/100.0) AS 'Cost ($)'",
                "location AS 'Location'",
                "notes AS 'Notes'"});
    }

    inline QString Lumber::groupedViewSQL()
    {
        return woodworks::infra::makeGroupedViewSQL(
            "display_lumber_grouped", "lumber",
            QStringList{
                "COUNT(*) AS 'Count'",
                "species AS 'Species'",
                "ROUND(length/16.0) AS 'Length (in)'",
                "printf('%d/4', thickness/4) AS 'Thickness'",
                "ROUND(width/16.0) AS 'Width (in)'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "CASE surfacing WHEN 0 THEN 'RGH' WHEN 1 THEN 'S1S' WHEN 2 THEN 'S2S' WHEN 3 THEN 'S3S' WHEN 4 THEN 'S4S' END AS 'Surfacing'",
                "ROUND(AVG(worth)/100.0,2) AS 'Avg Cost ($)'"},
            QStringList{
                "species",
                "ROUND(length/16.0)",
                "printf('%d/4', thickness/4)",
                "ROUND(width/16.0)",
                "drying",
                "surfacing"});
    }

    inline QString Lumber::insertSQL()
    {
        return "INSERT INTO lumber (species, length, width, thickness, drying, surfacing, worth, location, notes, image) VALUES (:species, :length, :width, :thickness, :drying, :surfacing, :worth, :location, :notes, :image)";
    }

    inline QString Lumber::updateSQL()
    {
        return "UPDATE lumber SET species = :species, length = :length, width = :width, thickness = :thickness, drying = :drying, surfacing = :surfacing, worth = :worth, location = :location, notes = :notes, image = :image WHERE id = :id";
    }

    inline QString Lumber::selectOneSQL() { return u8R"(SELECT * FROM lumber WHERE id=:id)"; }
    inline QString Lumber::selectAllSQL() { return u8R"(SELECT * FROM lumber)"; }

    inline QString Lumber::deleteSQL() { return u8R"(DELETE FROM lumber WHERE id=:id)"; }

    inline void Lumber::bindForInsert(QSqlQuery &q, const Lumber &l)
    {
        q.bindValue(":species", QString::fromStdString(l.species.name));
        q.bindValue(":length", l.length.toTicks());
        q.bindValue(":width", l.width.toTicks());
        q.bindValue(":thickness", l.thickness.toTicks());
        q.bindValue(":drying", static_cast<int>(l.drying));
        q.bindValue(":surfacing", static_cast<int>(l.surfacing));
        q.bindValue(":worth", static_cast<int>(l.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(l.location));
        q.bindValue(":notes", QString::fromStdString(l.notes));
        q.bindValue(":image", l.imageBuffer);
    }

    inline void Lumber::bindForUpdate(QSqlQuery &q, const Lumber &l)
    {
        q.bindValue(":species", QString::fromStdString(l.species.name));
        q.bindValue(":length", l.length.toTicks());
        q.bindValue(":width", l.width.toTicks());
        q.bindValue(":thickness", l.thickness.toTicks());
        q.bindValue(":drying", static_cast<int>(l.drying));
        q.bindValue(":surfacing", static_cast<int>(l.surfacing));
        q.bindValue(":worth", static_cast<int>(l.worth.toCents()));
        q.bindValue(":location", QString::fromStdString(l.location));
        q.bindValue(":notes", QString::fromStdString(l.notes));
        q.bindValue(":image", l.imageBuffer);
        q.bindValue(":id", l.id.id);
    }

    inline Lumber Lumber::fromRecord(const QSqlRecord &record)
    {
        Lumber lumber;
        lumber.id = Id{record.value("id").toInt()};
        lumber.species = Species{record.value("species").toString().toStdString()};
        lumber.length = Length::fromTicks(record.value("length").toDouble());
        lumber.width = Length::fromTicks(record.value("width").toDouble());
        lumber.thickness = Length::fromTicks(record.value("thickness").toDouble());
        lumber.drying = static_cast<Drying>(record.value("drying").toInt());
        lumber.surfacing = static_cast<LumberSurfacing>(record.value("surfacing").toInt());
        lumber.worth = Dollar{record.value("worth").toInt()};
        lumber.location = record.value("location").toString().toStdString();
        lumber.notes = record.value("notes").toString().toStdString();
        lumber.imageBuffer = record.value("image").toByteArray();
        return lumber;
    }
}