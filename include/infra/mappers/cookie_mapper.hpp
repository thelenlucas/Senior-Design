#pragma once

#include "domain/cookie.hpp"
#include "view_helpers.hpp"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>

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
                notes TEXT,
                image BLOB
            )
        )";
    }

    inline QString Cookie::individualViewSQL() {
        return woodworks::infra::makeIndividualViewSQL(
            "display_cookies", "cookies",
            QStringList{
                "id AS 'ID'",
                "species AS 'Species'",
                "ROUND(length/16.0,2) AS 'Thickness (in)'",
                "ROUND(diameter/16.0,2) AS 'Diameter (in)'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "printf('%.2f',worth/100.0) AS 'Worth ($)'",
                "location AS 'Location'",
                "notes AS 'Notes'"
            }
        );
    }

    inline QString Cookie::groupedViewSQL() {
        return woodworks::infra::makeGroupedViewSQL(
            "display_cookies_grouped", "cookies",
            QStringList{
                "COUNT(*) AS 'Count'",
                "species AS 'Species'",
                "ROUND(length/16.0,2) AS 'Thickness (in)'",
                "ROUND(diameter/16.0,2) AS 'Diameter (in)'",
                "CASE drying WHEN 0 THEN 'Green' WHEN 1 THEN 'Kiln Dried' WHEN 2 THEN 'Air Dried' WHEN 3 THEN 'Kiln & Air Dried' END AS 'Drying'",
                "ROUND(AVG(worth)/100.0,2) AS 'Avg Worth ($)'"
            },
            QStringList{
                "species",
                "ROUND(length/16.0,2)",
                "ROUND(diameter/16.0,2)",
                "drying"
            }
        );
    }

    inline QString Cookie::insertSQL()
    {
        return "INSERT INTO cookies (species, length, diameter, drying, worth, location, notes, image) VALUES (:species, :length, :diameter, :drying, :worth, :location, :notes, :image)";
    }

    inline QString Cookie::updateSQL()
    {
        return "UPDATE cookies SET species = :species, length = :length, diameter = :diameter, drying = :drying, worth = :worth, location = :location, notes = :notes, image = :image WHERE id = :id";
    }

    inline QString Cookie::selectOneSQL() { return u8R"(SELECT * FROM cookies WHERE id=:id)"; }
    inline QString Cookie::selectAllSQL() { return u8R"(SELECT * FROM cookies)"; }

    // Add delete SQL
    inline QString Cookie::deleteSQL() { return u8R"(DELETE FROM cookies WHERE id=:id)"; }

    inline void Cookie::bindForInsert(QSqlQuery& q, const Cookie& cookie)
    {
        q.bindValue(":species", QString::fromStdString(cookie.species.name));
        q.bindValue(":length", cookie.length.toTicks());
        q.bindValue(":diameter", cookie.diameter.toTicks());
        q.bindValue(":drying", static_cast<int>(cookie.drying));
        q.bindValue(":worth", cookie.worth.cents);
        q.bindValue(":location", QString::fromStdString(cookie.location));
        q.bindValue(":notes", QString::fromStdString(cookie.notes));
        q.bindValue(":image", cookie.imageBuffer);
    }

    inline void Cookie::bindForUpdate(QSqlQuery& q, const Cookie& cookie)
    {
        q.bindValue(":species", QString::fromStdString(cookie.species.name));
        q.bindValue(":length", cookie.length.toTicks());
        q.bindValue(":diameter", cookie.diameter.toTicks());
        q.bindValue(":drying", static_cast<int>(cookie.drying));
        q.bindValue(":worth", cookie.worth.cents);
        q.bindValue(":location", QString::fromStdString(cookie.location));
        q.bindValue(":notes", QString::fromStdString(cookie.notes));
        q.bindValue(":image", cookie.imageBuffer);
        q.bindValue(":id", cookie.id.id);
    }

    inline Cookie Cookie::fromRecord(const QSqlRecord& record)
    {
        Cookie cookie;
        cookie.id = Id{record.value("id").toInt()};
        cookie.species = Species{record.value("species").toString().toStdString()};
        cookie.length = Length::fromTicks(record.value("length").toDouble());
        cookie.diameter = Length::fromTicks(record.value("diameter").toDouble());
        cookie.drying = static_cast<Drying>(record.value("drying").toInt());
        cookie.worth = Dollar{record.value("worth").toInt()};
        cookie.location = record.value("location").toString().toStdString();
        cookie.notes = record.value("notes").toString().toStdString();
        cookie.imageBuffer = record.value("image").toByteArray();
        return cookie;
    }
}