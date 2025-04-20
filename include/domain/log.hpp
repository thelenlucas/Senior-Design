#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>

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

        // Validity check
        bool isValid() const noexcept
        {
            return !species.name.empty() && length.toTicks() > 0 && diameter.toTicks() > 0 && quality.isValid();
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