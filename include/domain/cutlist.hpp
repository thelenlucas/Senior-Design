/**
 * @file cutlist.hpp
 * @brief Defines CustomCut struct for custom project part cuts and database mapping.
 */

#pragma once

#include <QString>
#include <QSqlQuery>
#include <string>

#include "types.hpp"
#include "units.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;

namespace woodworks::domain
{
    /**
     * @struct CustomCut
     * @brief Represents a custom cut with attributes such as project, part, dimensions, species, progress, and notes.
     */
    struct CustomCut
    {
        /**
         * @brief Unique identifier for the custom cut.
         */
        Id id;

        /**
         * @brief Name of the project associated with the custom cut.
         */
        std::string project;

        /**
         * @brief Name of the part being cut.
         */
        std::string part;

        /**
         * @brief Code associated with the custom cut.
         */
        std::string code;

        /**
         * @brief Quantity of the custom cut required.
         */
        int quantity;

        /**
         * @brief Thickness of the custom cut.
         */
        Length t;

        /**
         * @brief Width of the custom cut.
         */
        Length w;

        /**
         * @brief Length of the custom cut.
         */
        Length l;

        /**
         * @brief Species of the material for the custom cut.
         */
        std::string species;

        /**
         * @brief Progress of the custom cut in rough state.
         */
        int progress_rough;

        /**
         * @brief Progress of the custom cut in finished state.
         */
        int progress_finished;

        /**
         * @brief Notes about the custom cut.
         */
        std::string notes;

        /**
         * @brief Image data buffer for the custom cut.
         */
        QByteArray imageBuffer;

        /**
         * @brief Creates an uninitialized CustomCut object.
         * @return An uninitialized CustomCut object.
         */
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

        /**
         * @brief Increments the rough progress of the custom cut.
         */
        inline void incrementRough()
        {
            if (progress_rough < quantity)
            {
                progress_rough++;
            }
        }

        /**
         * @brief Increments the finished progress of the custom cut.
         */
        inline void incrementFinished()
        {
            if (progress_finished < progress_rough)
            {
                progress_finished++;
            }
        }

        // ---- Mapping -----

        /**
         * @brief Generates the SQL statement for creating the database table.
         * @return A QString containing the SQL statement.
         */
        static QString createDbSQL();

        /**
         * @brief Generates the SQL query for viewing the cutlist of a specific project.
         * @param project The name of the project.
         * @return A QString containing the SQL query.
         */
        static QString cutlistViewSQLQuery(const QString &project);

        /**
         * @brief Generates the SQL statement for viewing individual custom cuts.
         * @return A QString containing the SQL statement.
         */
        static QString individualViewSQL();

        /**
         * @brief Generates the SQL statement for viewing grouped custom cuts.
         * @return A QString containing the SQL statement.
         */
        static QString groupedViewSQL();

        /**
         * @brief Generates the SQL statement for inserting a custom cut.
         * @return A QString containing the SQL statement.
         */
        static QString insertSQL();

        /**
         * @brief Generates the SQL statement for updating a custom cut.
         * @return A QString containing the SQL statement.
         */
        static QString updateSQL();

        /**
         * @brief Generates the SQL statement for selecting a single custom cut.
         * @return A QString containing the SQL statement.
         */
        static QString selectOneSQL();

        /**
         * @brief Generates the SQL statement for selecting all custom cuts.
         * @return A QString containing the SQL statement.
         */
        static QString selectAllSQL();

        /**
         * @brief Generates the SQL statement for deleting a custom cut.
         * @return A QString containing the SQL statement.
         */
        static QString deleteSQL();

        /**
         * @brief Binds the custom cut data to a QSqlQuery for insertion.
         * @param query The QSqlQuery object to bind data to.
         * @param customCut The CustomCut object containing the data.
         */
        static void bindForInsert(QSqlQuery &query, const CustomCut &customCut);

        /**
         * @brief Binds the custom cut data to a QSqlQuery for updating.
         * @param query The QSqlQuery object to bind data to.
         * @param customCut The CustomCut object containing the data.
         */
        static void bindForUpdate(QSqlQuery &query, const CustomCut &customCut);

        /**
         * @brief Creates a CustomCut object from a QSqlRecord.
         * @param record The QSqlRecord containing the custom cut data.
         * @return A CustomCut object populated with the data from the record.
         */
        static CustomCut fromRecord(const QSqlRecord &record);

        /**
         * @brief Retrieves all existing projects.
         * @return A vector of strings containing the names of all projects.
         */
        static std::vector<std::string> allProjects();
    };
}

#include "infra/mappers/cutlist_mapper.hpp"