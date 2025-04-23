#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>

#include "sales/product.hpp"

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

        inline static Lumber uninitialized() {
            return Lumber{
                .id = Id{ -1 },
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

        // HTML
        sales::Product toProduct();

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

        static bool matches(const Lumber& item, const Lumber& example) noexcept {
            return item.species.name == example.species.name &&
                   item.thickness.toTicks() == example.thickness.toTicks() &&
                   item.width.toTicks() == example.width.toTicks() &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.drying == example.drying &&
                   item.surfacing == example.surfacing &&
                   item.location == example.location;
        }
    };
}