#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

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

        // ---- Mapping -----
        static QString createDbSQL();
        static QString individualViewSQL();
        static QString insertSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();
        static void bindForInsert(QSqlQuery&, const Lumber&);
        static void bindForUpdate(QSqlQuery&, const Lumber&);
        static Lumber fromRecord(const QSqlRecord&);
    };
}