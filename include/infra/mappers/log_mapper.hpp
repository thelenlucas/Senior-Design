#pragma once
#include "domain/log.hpp"
#include "view_helpers.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain {
    inline QString Log::createDbSQL() {
        return u8R"(
            CREATE TABLE IF NOT EXISTS logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                species TEXT NOT NULL,
                length REAL NOT NULL,
                diameter REAL NOT NULL,
                quality INTEGER NOT NULL,
                drying INTEGER NOT NULL,
                cost INTEGER NOT NULL,
                location TEXT,
                notes TEXT,
                image BLOB
            )
        )";
    }

    inline QString Log::individualViewSQL() {
        return woodworks::infra::makeIndividualViewSQL(
            "display_logs", "logs",
            QStringList{
                "id AS 'ID'", 
                "species AS 'Species'", 
                "ROUND(length/192.0,2) AS 'Length (ft)'", 
                "ROUND(diameter/16.0,2) AS 'Diameter (in)'", 
                "quality AS 'Quality'", 
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'", 
                "printf('%.2f',cost/100.0) AS 'Cost ($)'", 
                "location AS 'Location'", 
                "notes AS 'Notes'"
            }
        );
    }

    inline QString Log::groupedViewSQL() {
        return woodworks::infra::makeGroupedViewSQL(
            "display_logs_grouped", "logs",
            QStringList{
                "COUNT(*) AS 'Count'", 
                "species AS 'Species'", 
                "ROUND(length/192.0,2) AS 'Length (ft)'", 
                "ROUND(diameter/16.0,2) AS 'Diameter (in)'", 
                "quality AS 'Quality'", 
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'", 
                "ROUND(AVG(cost)/100.0,2) AS 'Avg Cost ($)'"
            },
            QStringList{
                "species", 
                "ROUND(length/192.0,2)", 
                "ROUND(diameter/16.0,2)", 
                "quality", 
                "drying"
            }
        );
    }

    inline QString Log::insertSQL()
    {
        return "INSERT INTO logs (species, length, diameter, quality, drying, cost, location, notes, image) VALUES (:species, :length, :diameter, :quality, :drying, :cost, :location, :notes, :image)";
    }

    inline QString Log::updateSQL()
    {
        return "UPDATE logs SET species = :species, length = :length, diameter = :diameter, quality = :quality, drying = :drying, cost = :cost, location = :location, notes = :notes, image = :image WHERE id = :id";
    }

    inline QString Log::selectOneSQL() { return u8R"(SELECT * FROM logs WHERE id=:id)"; }

    inline QString Log::selectAllSQL() { return u8R"(SELECT * FROM logs)"; }

    inline QString Log::deleteSQL() { return u8R"(DELETE FROM logs WHERE id=:id)"; }

    inline void Log::bindForInsert(QSqlQuery& q, const Log& log)
    {
        q.bindValue(":species", QString::fromStdString(log.species.name));
        q.bindValue(":length", log.length.toTicks());
        q.bindValue(":diameter", log.diameter.toTicks());
        q.bindValue(":quality", log.quality.value);
        q.bindValue(":drying", static_cast<int>(log.drying));
        q.bindValue(":cost", log.cost.cents);
        q.bindValue(":location", QString::fromStdString(log.location));
        q.bindValue(":notes", QString::fromStdString(log.notes));
        q.bindValue(":image", log.imageBuffer);
    }

    inline void Log::bindForUpdate(QSqlQuery& q, const Log& log)
    {
        q.bindValue(":species", QString::fromStdString(log.species.name));
        q.bindValue(":length", log.length.toTicks());
        q.bindValue(":diameter", log.diameter.toTicks());
        q.bindValue(":quality", log.quality.value);
        q.bindValue(":drying", static_cast<int>(log.drying));
        q.bindValue(":cost", log.cost.cents);
        q.bindValue(":location", QString::fromStdString(log.location));
        q.bindValue(":notes", QString::fromStdString(log.notes));
        q.bindValue(":image", log.imageBuffer);
        q.bindValue(":id", log.id.id);
    }

    inline Log Log::fromRecord(const QSqlRecord& record)
    {
        Log log;
        log.id = Id{record.value("id").toInt()};
        log.species = Species{record.value("species").toString().toStdString()};
        log.length = Length::fromTicks(record.value("length").toDouble());
        log.diameter = Length::fromTicks(record.value("diameter").toDouble());
        log.quality = Quality(record.value("quality").toInt());
        log.drying = static_cast<Drying>(record.value("drying").toInt());
        log.cost = Dollar{record.value("cost").toInt()};
        log.location = record.value("location").toString().toStdString();
        log.notes = record.value("notes").toString().toStdString();
        log.imageBuffer = record.value("image").toByteArray();
        return log;
    }
}