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

        static Cookie uninitialized() noexcept
        {
            return Cookie{
                .id = Id{-1},
                .species = Species{""},
                .length = Length::fromTicks(0),
                .diameter = Length::fromTicks(0),
                .drying = Drying::GREEN,
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
        static QString selectOneSQL();
        static QString selectAllSQL();
        static QString deleteSQL();

        static void bindForInsert(QSqlQuery&, const Cookie&);
        static void bindForUpdate(QSqlQuery&, const Cookie&);
        static Cookie fromRecord(const QSqlRecord&);
    };
}