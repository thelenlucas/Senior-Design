/**
 * @file firewood.hpp
 * @brief Defines the Firewood struct representing pieces of firewood and database mapping utilities.
 */

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

namespace woodworks::domain
{
    /**
     * @struct Firewood
     * @brief Represents a piece of firewood with attributes such as species, volume, drying state, cost, location, notes, and image data.
     */
    struct Firewood
    {
        /**
         * @brief Unique identifier for the firewood bundle.
         */
        Id id{-1};

        /**
         * @brief Species of the original log.
         */
        Species species{""};

        /**
         * @brief Volume of the firewood in cubic feet.
         */
        double cubicFeet{0.0};

        /**
         * @brief Drying state of the firewood.
         */
        Drying drying{Drying::GREEN};

        /**
         * @brief Cost of the firewood in cents.
         */
        Dollar cost{0};

        /**
         * @brief Storage location of the firewood.
         */
        std::string location{""};

        /**
         * @brief Additional notes about the firewood.
         */
        std::string notes{""};

        /**
         * @brief Image data buffer for the firewood.
         */
        QByteArray imageBuffer{};

        /**
         * @brief Converts the volume in cubic feet to chords (1 chord = 128 cubic feet).
         * @return Volume in chords.
         */
        double toChords() const noexcept
        {
            return cubicFeet / 128.0;
        }

        /**
         * @brief Creates an uninitialized Firewood object with default values.
         * @return An uninitialized Firewood object.
         */
        static Firewood uninitialized()
        {
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

        /**
         * @brief Converts the firewood to a sales::Product representation.
         * @return A sales::Product representing the firewood.
         */
        sales::Product toProduct();

        // --- Mapping -----

        /**
         * @brief Generates the SQL for creating the firewood table.
         * @return SQL statement as QString.
         */
        static QString createDbSQL();

        /**
         * @brief SQL view for individual firewood entries.
         * @return SQL statement as QString.
         */
        static QString individualViewSQL();

        /**
         * @brief SQL view for grouped firewood entries.
         * @return SQL statement as QString.
         */
        static QString groupedViewSQL();

        /**
         * @brief SQL statement for inserting a firewood record.
         * @return SQL statement as QString.
         */
        static QString insertSQL();

        /**
         * @brief SQL statement for updating a firewood record.
         * @return SQL statement as QString.
         */
        static QString updateSQL();

        /**
         * @brief SQL statement for deleting a firewood record.
         * @return SQL statement as QString.
         */
        static QString deleteSQL();

        /**
         * @brief SQL statement for selecting one firewood record.
         * @return SQL statement as QString.
         */
        static QString selectOneSQL();

        /**
         * @brief SQL statement for selecting all firewood records.
         * @return SQL statement as QString.
         */
        static QString selectAllSQL();

        /**
         * @brief Binds firewood data to a QSqlQuery for insertion.
         * @param query The QSqlQuery to bind to.
         * @param fw The Firewood instance containing data.
         */
        static void bindForInsert(QSqlQuery &, const Firewood &);

        /**
         * @brief Binds firewood data to a QSqlQuery for updating.
         * @param query The QSqlQuery to bind to.
         * @param fw The Firewood instance containing data.
         */
        static void bindForUpdate(QSqlQuery &, const Firewood &);

        /**
         * @brief Constructs a Firewood object from a QSqlRecord.
         * @param record The record containing firewood data.
         * @return Populated Firewood object.
         */
        static Firewood fromRecord(const QSqlRecord &);

        /**
         * @brief Checks if two Firewood objects match on key fields.
         * @param item The Firewood to compare.
         * @param example The example Firewood to match against.
         * @return True if matching, otherwise false.
         */
        static bool matches(const Firewood &item, const Firewood &example) noexcept
        {
            return item.species.name == example.species.name &&
                   item.location == example.location &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}