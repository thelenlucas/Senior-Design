#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct LiveEdgeSlab {
        // Unique id per slab
        Id id;
        // Species of the slab
        Species species;
        // Length of the slab
        Length length;
        // Width of the slab
        Length width;
        // Thickness of the slab
        Length thickness;
        // Drying
        Drying drying;
        // Surfacing
        SlabSurfacing surfacing;
        // Dollar worth
        Dollar worth;
        // Location
        std::string location;
        // Notes
        std::string notes;

        // ---- Mapping -----
        static QString createDbSQL();
        static QString insertSQL();
        static QString individualViewSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();

        static void bindForInsert(QSqlQuery&, const LiveEdgeSlab&);
        static void bindForUpdate(QSqlQuery&, const LiveEdgeSlab&);
        static LiveEdgeSlab fromRecord(const QSqlRecord&);
    };
}