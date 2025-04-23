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
    struct LiveEdgeSlab {
        // Unique id per slab
        Id id{-1};
        // Species of the slab
        Species species{""};
        // Length of the slab
        Length length{Length::fromTicks(0)};
        // Width of the slab
        Length width{Length::fromTicks(0)};
        // Thickness of the slab
        Length thickness{Length::fromTicks(0)};
        // Drying
        Drying drying{Drying::GREEN};
        // Surfacing
        SlabSurfacing surfacing{SlabSurfacing::RGH};
        // Dollar worth
        Dollar worth{0};
        // Location
        std::string location{""};
        // Notes
        std::string notes{""};
        QByteArray imageBuffer{};  // image data buffer

        static LiveEdgeSlab uninitialized() {
            LiveEdgeSlab slab;
            slab.id = Id{-1};
            slab.species = Species{""};
            slab.length = Length::fromTicks(0);
            slab.width = Length::fromTicks(0);
            slab.thickness = Length::fromTicks(0);
            slab.drying = Drying::GREEN;
            slab.surfacing = SlabSurfacing::RGH;
            slab.worth = Dollar{0};
            slab.location = "";
            slab.notes = "";
            slab.imageBuffer = QByteArray();
            return slab;
        }

        // HTML
        sales::Product toProduct();

        // ---- Mapping -----
        static QString createDbSQL();
        static QString insertSQL();
        static QString individualViewSQL();
        static QString groupedViewSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();
        static QString deleteSQL();

        static void bindForInsert(QSqlQuery&, const LiveEdgeSlab&);
        static void bindForUpdate(QSqlQuery&, const LiveEdgeSlab&);
        static LiveEdgeSlab fromRecord(const QSqlRecord&);

        static bool matches(const LiveEdgeSlab& item, const LiveEdgeSlab& example) noexcept {
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