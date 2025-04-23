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
        Id id{-1};
        Species species{""};
        Length length{Length::fromTicks(0)};
        Length width{Length::fromTicks(0)};
        Length thickness{Length::fromTicks(0)};
        Drying drying{Drying::GREEN};
        LumberSurfacing surfacing{LumberSurfacing::S4S};
        Dollar worth{0};
        std::string location{""};
        std::string notes{""};
        QByteArray imageBuffer{};  // image data buffer

        inline static Lumber uninitialized() {
            Lumber lumber;
            lumber.id = Id{-1};
            lumber.species = Species{""};
            lumber.length = Length::fromTicks(0);
            lumber.width = Length::fromTicks(0);
            lumber.thickness = Length::fromTicks(0);
            lumber.drying = Drying::GREEN;
            lumber.surfacing = LumberSurfacing::S4S;
            lumber.worth = Dollar{0};
            lumber.location = "";
            lumber.notes = "";
            lumber.imageBuffer = QByteArray();
            return lumber;
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