#pragma once
#include "domain/firewood.hpp"
#include "view_helpers.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain
{
    inline QString Firewood::createDbSQL()
    {
        return u8R"(
            CREATE TABLE IF NOT EXISTS firewood (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                species TEXT NOT NULL,
                cubicFeet REAL NOT NULL,
                drying INTEGER NOT NULL,
                cost INTEGER NOT NULL,
                location TEXT,
                notes TEXT,
                image BLOB
            )
        )";
    }

    // No individual view for firewood, but we still need to define the function
    inline QString Firewood::individualViewSQL()
    {
        return woodworks::infra::makeIndividualViewSQL(
            "display_firewood", "firewood",
            QStringList{
                "id AS 'ID'",
                "species AS 'Species'",
                "ROUND(cubicFeet,2) AS 'Cubic Feet'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "ROUND(cost/100.0,2) AS 'Cost ($)'",
                "location AS 'Location'",
                "notes AS 'Notes'"});
    }

    inline QString Firewood::groupedViewSQL()
    {
        return woodworks::infra::makeGroupedViewSQL(
            "display_firewood_grouped", "firewood",
            QStringList{
                "species AS 'Species'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "ROUND(sum(cubicFeet),2) AS 'Cubic Feet'",
                "ROUND(sume(cubicFeet)/128.0,2) AS 'Chords'",
                "ROUND(SUM(cost)/100.0,2) AS 'Cost ($)'"},
            QStringList{
                "species",
                "drying"});
    }
    inline QString Firewood::insertSQL()
    {
        return "INSERT INTO firewood (species, cubicFeet, drying, cost, location, notes, image) VALUES (:species, :cubicFeet, :drying, :cost, :location, :notes, :image)";
    }
    inline QString Firewood::updateSQL()
    {
        return "UPDATE firewood SET species = :species, cubicFeet = :cubicFeet, drying = :drying, cost = :cost, location = :location, notes = :notes, image = :image WHERE id = :id";
    }
    inline QString Firewood::selectOneSQL() { return u8R"(SELECT * FROM firewood WHERE id=:id)"; }
    inline QString Firewood::selectAllSQL() { return u8R"(SELECT * FROM firewood)"; }
    inline QString Firewood::deleteSQL() { return u8R"(DELETE FROM firewood WHERE id=:id)"; }
    inline void Firewood::bindForInsert(QSqlQuery &q, const Firewood &firewood)
    {
        q.bindValue(":species", QString::fromStdString(firewood.species.name));
        q.bindValue(":cubicFeet", firewood.cubicFeet);
        q.bindValue(":drying", static_cast<int>(firewood.drying));
        q.bindValue(":cost", firewood.cost.cents);
        q.bindValue(":location", QString::fromStdString(firewood.location));
        q.bindValue(":notes", QString::fromStdString(firewood.notes));
        q.bindValue(":image", firewood.imageBuffer);
    }

    inline void Firewood::bindForUpdate(QSqlQuery &q, const Firewood &firewood)
    {
        q.bindValue(":id", firewood.id.id);
        q.bindValue(":species", QString::fromStdString(firewood.species.name));
        q.bindValue(":cubicFeet", firewood.cubicFeet);
        q.bindValue(":drying", static_cast<int>(firewood.drying));
        q.bindValue(":cost", firewood.cost.cents);
        q.bindValue(":location", QString::fromStdString(firewood.location));
        q.bindValue(":notes", QString::fromStdString(firewood.notes));
        q.bindValue(":image", firewood.imageBuffer);
    }

    inline Firewood Firewood::fromRecord(const QSqlRecord &record)
    {
        return Firewood{
            .id = {record.value("id").toInt()},
            .species = {record.value("species").toString().toStdString()},
            .cubicFeet = record.value("cubicFeet").toDouble(),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .cost = {record.value("cost").toInt()},
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString(),
            .imageBuffer = record.value("image").toByteArray()};
    }
}