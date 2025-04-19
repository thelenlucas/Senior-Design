#pragma once

#include "units.hpp"
#include "types.hpp"
#include <QString>
#include <string>
#include <QSqlQuery>

using namespace domain::types;
using namespace domain::imperial;

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
        std::string location;
        std::string notes;

        // ---- Mapping -----
        static QString insertSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();

        static void bindForInsert(QSqlQuery&, const Log&);
        static void bindForUpdate(QSqlQuery&, const Log&);
        static Log fromRecord(const QSqlRecord&);
    };
}