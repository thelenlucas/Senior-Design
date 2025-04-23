#pragma once

#include <QString>
#include <QSqlQuery>
#include <string>

#include "types.hpp"
#include "units.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;

namespace woodworks::domain {
    struct CustomCut
    {
        Id id;
        std::string project;
        std::string part;
        std::string code;
        int quantity;
        Length t;
        Length w;
        Length l;
        std::string species;
        int progress_rough;
        int progress_finished;
        std::string notes;
        QByteArray imageBuffer;

        static CustomCut uninitialized() noexcept
        {
            return CustomCut{
                Id{0},
                std::string{},
                std::string{},
                std::string{},
                0,
                Length::fromTicks(0),
                Length::fromTicks(0),
                Length::fromTicks(0),
                std::string{},
                0,
                0,
                std::string{},
                QByteArray{},
            };
        }

        // ---- Mapping -----
        static QString createDbSQL();
        static QString cutlistViewSQLQuery(const QString& project);
        static QString individualViewSQL();
        static QString groupedViewSQL();
        static QString insertSQL();
        static QString updateSQL();
        static QString selectOneSQL();
        static QString selectAllSQL();
        static QString deleteSQL();
        static void bindForInsert(QSqlQuery&, const CustomCut&);
        static void bindForUpdate(QSqlQuery&, const CustomCut&);
        static CustomCut fromRecord(const QSqlRecord&);

        // Returns all extant projects
        static std::vector<std::string> allProjects();
    };
}

#include "infra/mappers/cutlist_mapper.hpp"