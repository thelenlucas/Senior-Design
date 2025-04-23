#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>
#include <QByteArray>
#include "domain/cookie.hpp"

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    struct Log {
        // Unique id per log
        Id id;
        // Species of the log
        Species species;
        // Length of the log
        Length length;
        // Diameter of the log
        Length diameter;
        // Quality (1-5)
        Quality quality;
        // Drying
        Drying drying;
        // Cost in cents
        Dollar cost;
        std::string location;
        std::string notes;
        QByteArray imageBuffer;  // image data buffer

        Log(Id id, Species species, Length length, Length diameter,
            Quality quality, Drying drying, Dollar cost, std::string location,
            std::string notes, QByteArray imageBuffer);

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
        void cutFirewood(Length cutLength);

        static Log uninitialized() noexcept
        {
            return Log(
                /*id*/ Id{-1},
                /*species*/ Species{""},
                /*length*/ Length::fromTicks(0),
                /*diameter*/ Length::fromTicks(0),
                /*quality*/ Quality{-1},
                /*drying*/ Drying::GREEN,
                /*cost*/ Dollar{0},
                /*location*/ "",
                /*notes*/ "",
                /*imageBuffer*/ QByteArray());
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
    };
}