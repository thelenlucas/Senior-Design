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

        static Lumber uninitialized() noexcept
        {
            return Lumber{
                .id = Id{-1},
                .species = Species{""},
                .length = Length::fromTicks(0),
                .width = Length::fromTicks(0),
                .thickness = Length::fromTicks(0),
                .drying = Drying::GREEN,
                .surfacing = LumberSurfacing::RGH,
                .worth = Dollar{0},
                .location = "",
                .notes = "",
                .imageBuffer = QByteArray(),
            };
        }

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