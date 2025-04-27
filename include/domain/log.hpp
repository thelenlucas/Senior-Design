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

namespace woodworks::domain
{
    /**
     * Represents a log with various attributes such as species, dimensions, quality, and cost.
     */
    struct Log
    {
        /** Unique identifier for the log */
        Id id{-1};
        /** Species of the log */
        Species species{""};
        /** Length of the log */
        Length length{Length::fromTicks(0)};
        /** Diameter of the log */
        Length diameter{Length::fromTicks(0)};
        /** Quality of the log (1-5) */
        Quality quality{Quality{-1}};
        /** Drying state of the log */
        Drying drying{Drying::GREEN};
        /** Cost of the log in cents */
        Dollar cost{0};
        /** Location of the log */
        std::string location{""};
        /** Additional notes about the log */
        std::string notes{""};
        /** Image data buffer for the log */
        QByteArray imageBuffer{};

        /**
         * Checks if the log is valid.
         * @return True if the log has valid attributes, false otherwise.
         */
        bool isValid() const noexcept
        {
            return !species.name.empty() && length.toTicks() > 0 && diameter.toTicks() > 0 && quality.isValid();
        }

        /**
         * Cuts a length off the log and updates the remaining length.
         * Deletes the log if the length becomes zero.
         * @param cutLength The length to cut off.
         * @return The worth of the cut length in dollars.
         */
        Dollar cut(Length cutLength);
        /**
         * Cuts a cookie from the log, inserts it into the database, and updates the log.
         * @param cutLength The length of the cookie to cut.
         * @return The resulting Cookie object.
         */
        Cookie cutCookie(Length cutLength);
        /**
         * Cuts a length of firewood from the log, inserts it into the database, and updates the log.
         * @param cutLength The length of firewood to cut.
         * @return The resulting Firewood object.
         */
        Firewood cutFirewood(Length cutLength);

        /**
         * Creates an uninitialized log with default values.
         * @return An uninitialized Log object.
         */
        static Log uninitialized()
        {
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

        /**
         * Generates the SQL statement for creating the database table.
         * @return The SQL create table statement.
         */
        static QString createDbSQL();

        /**
         * Generates the SQL statement for viewing individual logs.
         * @return The SQL individual view statement.
         */
        static QString individualViewSQL();

        /**
         * Generates the SQL statement for viewing grouped logs.
         * @return The SQL grouped view statement.
         */
        static QString groupedViewSQL();

        /**
         * Generates the SQL statement for inserting a log into the database.
         * @return The SQL insert statement.
         */
        static QString insertSQL();

        /**
         * Generates the SQL statement for updating a log in the database.
         * @return The SQL update statement.
         */
        static QString updateSQL();

        /**
         * Generates the SQL statement for selecting a single log from the database.
         * @return The SQL select one statement.
         */
        static QString selectOneSQL();

        /**
         * Generates the SQL statement for selecting all logs from the database.
         * @return The SQL select all statement.
         */
        static QString selectAllSQL();

        /**
         * Generates the SQL statement for deleting a log from the database.
         * @return The SQL delete statement.
         */
        static QString deleteSQL();

        /**
         * Binds the log attributes to a SQL insert query.
         * @param query The SQL query object.
         * @param log The log to bind.
         */
        static void bindForInsert(QSqlQuery &query, const Log &log);

        /**
         * Binds the log attributes to a SQL update query.
         * @param query The SQL query object.
         * @param log The log to bind.
         */
        static void bindForUpdate(QSqlQuery &query, const Log &log);

        /**
         * Creates a Log object from a database record.
         * @param record The database record.
         * @return The resulting Log object.
         */
        static Log fromRecord(const QSqlRecord &record);

        /**
         * Checks if a log matches an example log based on specific attributes.
         * @param item The log to check.
         * @param example The example log to match against.
         * @return True if the logs match, false otherwise.
         */
        static bool matches(const Log &item, const Log &example) noexcept
        {
            return item.species.name == example.species.name &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.diameter.toTicks() == example.diameter.toTicks() &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}