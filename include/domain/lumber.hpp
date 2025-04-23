#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Lumber {
        Id id;
        Species species;
        Length length;
        Length width;
        Length thickness;
        Drying drying;
        LumberSurfacing surfacing;
        Dollar worth;
        std::string location;
        std::string notes;
        QByteArray imageBuffer;  // image data buffer

        Lumber(Id id, Species species, Length length, Length width,
               Length thickness, Drying drying, LumberSurfacing surfacing,
               Dollar worth, std::string location, std::string notes,
               QByteArray imageBuffer) : 
              id(id), species(std::move(species)), length(length), width(width),
              thickness(thickness), drying(drying), surfacing(surfacing),
              worth(worth), location(std::move(location)),
              notes(std::move(notes)), imageBuffer(std::move(imageBuffer))
        {}

        // ---- Mapping -----
        static QString createDbSQL();
        static QString individualViewSQL();
        static QString groupedViewSQL();
        static QString insertSQL();
        static QString updateSQL();
        static QString deleteSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();
        static void bindForInsert(QSqlQuery&, const Lumber&);
        static void bindForUpdate(QSqlQuery&, const Lumber&);
        static Lumber fromRecord(const QSqlRecord&);
    };
}