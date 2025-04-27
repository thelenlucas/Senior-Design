#pragma once

#include "types.hpp"
#include "units.hpp"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QByteArray>

#include "sales/product.hpp"

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;

namespace woodworks::domain {
    /**
     * Represents a piece of lumber with various attributes such as species, dimensions, drying state, and surfacing.
     */
    struct Lumber {
        /** Unique identifier for the lumber */
        Id id{-1};
        /** Species of the lumber */
        Species species{""};
        /** Length of the lumber */
        Length length{Length::fromTicks(0)};
        /** Width of the lumber */
        Length width{Length::fromTicks(0)};
        /** Thickness of the lumber */
        Length thickness{Length::fromTicks(0)};
        /** Drying state of the lumber */
        Drying drying{Drying::GREEN};
        /** Surfacing type of the lumber */
        LumberSurfacing surfacing{LumberSurfacing::S4S};
        /** Worth of the lumber in dollars */
        Dollar worth{0};
        /** Location of the lumber */
        std::string location{""};
        /** Additional notes about the lumber */
        std::string notes{""};
        /** Image data buffer for the lumber */
        QByteArray imageBuffer{};

        /**
         * Creates an uninitialized Lumber object with default values.
         * @return An uninitialized Lumber object.
         */
        inline static Lumber uninitialized() {
            Lumber lumber;
            lumber.id = Id{-1};
            lumber.species = Species{""};
            lumber.length = Length::fromTicks(0);
            lumber.width = Length::fromTicks(0);
            lumber.thickness = Length::fromTicks(0);
            lumber.drying = Drying::GREEN;
            lumber.surfacing = LumberSurfacing::S4S;
            lumber.worth = Dollar{0};
            lumber.location = "";
            lumber.notes = "";
            lumber.imageBuffer = QByteArray();
            return lumber;
        }

        /**
         * Converts the lumber to a sales product.
         * @return The corresponding sales::Product object.
         */
        sales::Product toProduct();

        // ---- Mapping -----

        /**
         * Generates the SQL statement for creating the database table.
         * @return The SQL create table statement.
         */
        static QString createDbSQL();

        /**
         * Generates the SQL statement for viewing individual lumber entries.
         * @return The SQL individual view statement.
         */
        static QString individualViewSQL();

        /**
         * Generates the SQL statement for viewing grouped lumber entries.
         * @return The SQL grouped view statement.
         */
        static QString groupedViewSQL();

        /**
         * Generates the SQL statement for inserting a lumber entry into the database.
         * @return The SQL insert statement.
         */
        static QString insertSQL();

        /**
         * Generates the SQL statement for updating a lumber entry in the database.
         * @return The SQL update statement.
         */
        static QString updateSQL();

        /**
         * Generates the SQL statement for deleting a lumber entry from the database.
         * @return The SQL delete statement.
         */
        static QString deleteSQL();

        /**
         * Generates the SQL statement for selecting a single lumber entry from the database.
         * @return The SQL select one statement.
         */
        static QString selectOneSQL();

        /**
         * Generates the SQL statement for selecting all lumber entries from the database.
         * @return The SQL select all statement.
         */
        static QString selectAllSQL();

        /**
         * Binds the lumber attributes to a SQL insert query.
         * @param query The SQL query object.
         * @param lumber The lumber to bind.
         */
        static void bindForInsert(QSqlQuery& query, const Lumber& lumber);

        /**
         * Binds the lumber attributes to a SQL update query.
         * @param query The SQL query object.
         * @param lumber The lumber to bind.
         */
        static void bindForUpdate(QSqlQuery& query, const Lumber& lumber);

        /**
         * Creates a Lumber object from a database record.
         * @param record The database record.
         * @return The resulting Lumber object.
         */
        static Lumber fromRecord(const QSqlRecord& record);

        /**
         * Checks if a lumber entry matches an example lumber based on specific attributes.
         * @param item The lumber entry to check.
         * @param example The example lumber to match against.
         * @return True if the lumber entries match, false otherwise.
         */
        static bool matches(const Lumber& item, const Lumber& example) noexcept {
            return item.species.name == example.species.name &&
                   item.thickness.toTicks() == example.thickness.toTicks() &&
                   item.width.toTicks() == example.width.toTicks() &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.drying == example.drying &&
                   item.surfacing == example.surfacing &&
                   item.location == example.location;
        }
    };
}