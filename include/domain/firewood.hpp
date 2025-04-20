#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>
#include <QByteArray>

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Firewood {
        // Unique id per bundle
        Id id;
        // Species of the original log
        Species species;
        // Cubic feet of the firewood
        double cubicFeet;
        // Drying
        Drying drying;
        // Cost in cents
        Dollar cost;
        std::string location;
        std::string notes;
        QByteArray imageBuffer;  // image data buffer

        // Chords - a unit of measure for firewood, equal to 128 cubic feet
        // 1 chord = 128 cubic feet
        double toChords() const noexcept
        {
            return cubicFeet / 128.0;
        }

        // --- Mapping -----
        static QString createDbSQL();
        static QString individualViewSQL();
        static QString groupedViewSQL();
        static QString insertSQL();
        static QString updateSQL();
        static QString deleteSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();

        static void bindForInsert(QSqlQuery&, const Firewood&);
        static void bindForUpdate(QSqlQuery&, const Firewood&);
        static Firewood fromRecord(const QSqlRecord&);
    };
}