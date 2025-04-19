#pragma once

#include "domain/cookie.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

namespace woodworks::domain {
    inline QString Cookie::createDbSQL() {
        return u8R"(
            CREATE TABLE IF NOT EXISTS cookies (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                species TEXT NOT NULL,
                length REAL NOT NULL,
                diameter REAL NOT NULL,
                drying INTEGER NOT NULL,
                worth INTEGER NOT NULL,
                location TEXT,
                notes TEXT
            )
        )";
    }

    inline QString Cookie::insertSQL()
    {
        return "INSERT INTO cookies (species, length, diameter, drying, worth, location, notes) VALUES (:species, :length, :diameter, :drying, :worth, :location, :notes)";
    }

    inline QString Cookie::updateSQL()
    {
        return "UPDATE cookies SET species = :species, length = :length, diameter = :diameter, drying = :drying, worth = :worth, location = :location, notes = :notes WHERE id = :id";
    }

    inline QString Cookie::selectOneSQL() { return u8R"(SELECT * FROM cookies WHERE id=:id)"; }
    inline QString Cookie::selectAllSQL() { return u8R"(SELECT * FROM cookies)"; }

    inline void Cookie::bindForInsert(QSqlQuery& q, const Cookie& cookie)
    {
        q.bindValue(":species", QString::fromStdString(cookie.species.name));
        q.bindValue(":length", cookie.length.toInches());
        q.bindValue(":diameter", cookie.diameter.toInches());
        q.bindValue(":drying", static_cast<int>(cookie.drying));
        q.bindValue(":worth", cookie.worth.cents);
        q.bindValue(":location", QString::fromStdString(cookie.location));
        q.bindValue(":notes", QString::fromStdString(cookie.notes));
    }

    inline void Cookie::bindForUpdate(QSqlQuery& q, const Cookie& cookie)
    {
        q.bindValue(":species", QString::fromStdString(cookie.species.name));
        q.bindValue(":length", cookie.length.toInches());
        q.bindValue(":diameter", cookie.diameter.toInches());
        q.bindValue(":drying", static_cast<int>(cookie.drying));
        q.bindValue(":worth", cookie.worth.cents);
        q.bindValue(":location", QString::fromStdString(cookie.location));
        q.bindValue(":notes", QString::fromStdString(cookie.notes));
        q.bindValue(":id", cookie.id.id);
    }

    inline Cookie Cookie::fromRecord(const QSqlRecord& record)
    {
        return {
            .id = {record.value("id").toInt()},
            .species = {record.value("species").toString().toStdString()},
            .length = Length::fromInches(record.value("length").toDouble()),
            .diameter = Length::fromInches(record.value("diameter").toDouble()),
            .drying = static_cast<Drying>(record.value("drying").toInt()),
            .worth = Dollar{record.value("worth").toInt()},
            .location = record.value("location").toString().toStdString(),
            .notes = record.value("notes").toString().toStdString()
        };
    }
}