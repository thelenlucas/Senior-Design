#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>
#include <QByteArray>
#include "domain/cookie.hpp"
#include "domain/firewood.hpp"

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Log {
        // Unique id per log
        Id id{-1};
        // Species of the log
        Species species{""};
        // Length of the log
        Length length{Length::fromTicks(0)};
        // Diameter of the log
        Length diameter{Length::fromTicks(0)};
        // Quality (1-5)
        Quality quality{Quality{-1}};
        // Drying
        Drying drying{Drying::GREEN};
        // Cost in cents
        Dollar cost{0};
        std::string location{""};
        std::string notes{""};
        QByteArray imageBuffer{};  // image data buffer

        // Validity check
        bool isValid() const noexcept
        {
            return !species.name.empty() && length.toTicks() > 0 && diameter.toTicks() > 0 && quality.isValid();
        }

        // Cuts a length off the log and updates the remaining length. Deletes the log if the length is zero. Returns the worth of the cut length.
        Dollar cut(Length cutLength);
        // Cuts a cookie from the log, inserts it into the database, and updates the log.
        Cookie cutCookie(Length cutLength);
        // Cuts a length of firewood from the log, inserts it into the database, and updates the log.
        Firewood cutFirewood(Length cutLength);

        static Log uninitialized() {
            Log log;
            log.id = Id{-1};
            log.species = Species{""};
            log.length = Length::fromTicks(0);
            log.diameter = Length::fromTicks(0);
            log.quality = Quality{-1};
            log.drying = Drying::GREEN;
            log.cost = Dollar{0};
            log.location = "";
            log.notes = "";
            log.imageBuffer = QByteArray();
            return log;
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

        static void bindForInsert(QSqlQuery&, const Log&);
        static void bindForUpdate(QSqlQuery&, const Log&);
        static Log fromRecord(const QSqlRecord&);

        static bool matches(const Log& item, const Log& example) noexcept {
            return item.species.name == example.species.name &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.diameter.toTicks() == example.diameter.toTicks() &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}