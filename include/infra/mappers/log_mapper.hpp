#pragma once
#include "domain/log.hpp"
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
                notes TEXT
            )
        )";
    }

    inline QString Log::insertSQL()
    {
        return "INSERT INTO logs (species, length, diameter, quality, drying, cost, location, notes) VALUES (:species, :length, :diameter, :quality, :drying, :cost, :location, :notes)";
    }

    inline QString Log::updateSQL()
    {
        return "UPDATE logs SET species = :species, length = :length, diameter = :diameter, quality = :quality, drying = :drying, cost = :cost, location = :location, notes = :notes WHERE id = :id";
    }

    inline QString Log::selectOneSQL() { return u8R"(SELECT * FROM logs WHERE id=:id)"; }

    inline QString Log::selectAllSQL() { return u8R"(SELECT * FROM logs)"; }

    inline void Log::bindForInsert(QSqlQuery& q, const Log& log)
    {
        q.bindValue(":species", QString::fromStdString(log.species.name));
        q.bindValue(":length", log.length.toInches());
        q.bindValue(":diameter", log.diameter.toInches());
        q.bindValue(":quality", log.quality.value);
        q.bindValue(":drying", static_cast<int>(log.drying));
        q.bindValue(":cost", log.cost.cents);
        q.bindValue(":location", QString::fromStdString(log.location));
        q.bindValue(":notes", QString::fromStdString(log.notes));
    }

    inline void Log::bindForUpdate(QSqlQuery& q, const Log& log)
    {
        q.bindValue(":species", QString::fromStdString(log.species.name));
        q.bindValue(":length", log.length.toInches());
        q.bindValue(":diameter", log.diameter.toInches());
        q.bindValue(":quality", log.quality.value);
        q.bindValue(":drying", static_cast<int>(log.drying));
        q.bindValue(":cost", log.cost.cents);
        q.bindValue(":location", QString::fromStdString(log.location));
        q.bindValue(":notes", QString::fromStdString(log.notes));
        q.bindValue(":id", log.id.id);
    }

    inline Log Log::fromRecord(const QSqlRecord& record)
    {
        Log log {
            .id = {record.value("id").toInt()},
            .species = {record.value("species").toString().toStdString()},
            .length = Length::fromInches(record.value("length").toDouble()),
            .diameter = Length::fromInches(record.value("diameter").toDouble()),
            .quality = Quality(record.value("quality").toInt()),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .cost = {Dollar(record.value("cost").toInt())},
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString(),
        };
        
        return log;
    }
}