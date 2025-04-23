#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>
#include <QByteArray>

#include "sales/product.hpp"

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Firewood {
        // Unique id per bundle
        Id id{-1};
        // Species of the original log
        Species species{""};
        // Cubic feet of the firewood
        double cubicFeet{0.0};
        // Drying
        Drying drying{Drying::GREEN};
        // Cost in cents
        Dollar cost{0};
        std::string location{""};
        std::string notes{""};
        QByteArray imageBuffer{};  // image data buffer

        // Chords - a unit of measure for firewood, equal to 128 cubic feet
        // 1 chord = 128 cubic feet
        double toChords() const noexcept
        {
            return cubicFeet / 128.0;
        }

        static Firewood uninitialized() {
            Firewood fw;
            fw.id = Id{-1};
            fw.species = Species{""};
            fw.cubicFeet = 0.0;
            fw.drying = Drying::GREEN;
            fw.cost = Dollar{0};
            fw.location = "";
            fw.notes = "";
            fw.imageBuffer = QByteArray();
            return fw;
        }

        // HTML
        sales::Product toProduct();

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

        static bool matches(const Firewood& item, const Firewood& example) noexcept {
            return item.species.name == example.species.name &&
                   item.location == example.location &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}