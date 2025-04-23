#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Cookie {
        // Unique id per cookie
        Id id;
        // Species of the cookie
        Species species;
        // Length of the cookie (along it's cylindrical axis)
        Length length;
        // Diameter of the cookie
        Length diameter;
        // Drying
        Drying drying;
        // Worth 
        Dollar worth;
        // Location
        std::string location;
        // Notes
        std::string notes;
        QByteArray imageBuffer;  // image data buffer

        Cookie(Id id, Species species, Length length, Length diameter,
               Drying drying, Dollar worth, std::string location,
               std::string notes, QByteArray imageBuffer) : 
              id(id), species(std::move(species)), length(length),
              diameter(diameter), drying(drying), worth(worth),
              location(std::move(location)), notes(std::move(notes)),
              imageBuffer(std::move(imageBuffer))
        {}

        // ---- Mapping -----
        static QString createDbSQL();
        static QString individualViewSQL();
        static QString groupedViewSQL();
        static QString insertSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();
        static QString deleteSQL();

        static void bindForInsert(QSqlQuery&, const Cookie&);
        static void bindForUpdate(QSqlQuery&, const Cookie&);
        static Cookie fromRecord(const QSqlRecord&);
    };
}