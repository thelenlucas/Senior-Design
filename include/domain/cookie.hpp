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
    struct Cookie {
        // Unique id per cookie
        Id id{-1};
        // Species of the cookie
        Species species{""};
        // Length of the cookie (along it's cylindrical axis)
        Length length{Length::fromTicks(0)};
        // Diameter of the cookie
        Length diameter{Length::fromTicks(0)};
        // Drying
        Drying drying{Drying::GREEN};
        // Worth 
        Dollar worth{0};
        // Location
        std::string location{""};
        // Notes
        std::string notes{""};
        QByteArray imageBuffer{};  // image data buffer

        static Cookie uninitialized() {
            Cookie cookie;
            cookie.id = Id{-1};
            cookie.species = Species{""};
            cookie.length = Length::fromTicks(0);
            cookie.diameter = Length::fromTicks(0);
            cookie.drying = Drying::GREEN;
            cookie.worth = Dollar{0};
            cookie.location = "";
            cookie.notes = "";
            cookie.imageBuffer = QByteArray();
            return cookie;
        }

        // HTML
        sales::Product toProduct();

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

        static bool matches(const Cookie& item, const Cookie& example) noexcept {
            return item.species.name == example.species.name &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.diameter.toTicks() == example.diameter.toTicks() &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}