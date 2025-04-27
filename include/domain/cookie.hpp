/**
 * @file cookie.hpp
 * @brief Defines the Cookie struct representing log slices with attributes and database mapping.
 */

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
     * @struct Cookie
     * @brief Represents a cookie with various attributes such as species, dimensions, drying state, worth, location, and notes.
     */
    struct Cookie {
        /**
         * @brief Unique id per cookie.
         */
        Id id{-1};

        /**
         * @brief Species of the cookie.
         */
        Species species{""};

        /**
         * @brief Length of the cookie (along its cylindrical axis).
         */
        Length length{Length::fromTicks(0)};

        /**
         * @brief Diameter of the cookie.
         */
        Length diameter{Length::fromTicks(0)};

        /**
         * @brief Drying state of the cookie.
         */
        Drying drying{Drying::GREEN};

        /**
         * @brief Worth of the cookie in dollars.
         */
        Dollar worth{0};

        /**
         * @brief Location of the cookie.
         */
        std::string location{""};

        /**
         * @brief Notes about the cookie.
         */
        std::string notes{""};

        /**
         * @brief Image data buffer for the cookie.
         */
        QByteArray imageBuffer{};

        /**
         * @brief Creates an uninitialized cookie object.
         * @return An uninitialized Cookie object.
         */
        static Cookie uninitialized() {
            Cookie cookie;
            cookie.id = Id{-1};
            cookie.species = Species{""};
            cookie.length = Length::fromTicks(0);
            cookie.diameter = Length::fromTicks(0);
            cookie.drying = Drying::GREEN;
            cookie.worth = Dollar{0};
            cookie.location = "";
            cookie.notes = "";
            cookie.imageBuffer = QByteArray();
            return cookie;
        }

        /**
         * @brief Converts the cookie to a sales::Product object.
         * @return A sales::Product representation of the cookie.
         */
        sales::Product toProduct();

        // ---- Mapping -----

        /**
         * @brief Generates the SQL statement for creating the database table.
         * @return A QString containing the SQL statement.
         */
        static QString createDbSQL();

        /**
         * @brief Generates the SQL statement for viewing individual cookies.
         * @return A QString containing the SQL statement.
         */
        static QString individualViewSQL();

        /**
         * @brief Generates the SQL statement for viewing grouped cookies.
         * @return A QString containing the SQL statement.
         */
        static QString groupedViewSQL();

        /**
         * @brief Generates the SQL statement for inserting a cookie.
         * @return A QString containing the SQL statement.
         */
        static QString insertSQL();

        /**
         * @brief Generates the SQL statement for updating a cookie.
         * @return A QString containing the SQL statement.
         */
        static QString updateSQL();

        /**
         * @brief Generates the SQL statement for selecting a single cookie.
         * @return A QString containing the SQL statement.
         */
        static QString selectOneSQL();

        /**
         * @brief Generates the SQL statement for selecting all cookies.
         * @return A QString containing the SQL statement.
         */
        static QString selectAllSQL();

        /**
         * @brief Generates the SQL statement for deleting a cookie.
         * @return A QString containing the SQL statement.
         */
        static QString deleteSQL();

        /**
         * @brief Binds the cookie data to a QSqlQuery for insertion.
         * @param query The QSqlQuery object to bind data to.
         * @param cookie The Cookie object containing the data.
         */
        static void bindForInsert(QSqlQuery& query, const Cookie& cookie);

        /**
         * @brief Binds the cookie data to a QSqlQuery for updating.
         * @param query The QSqlQuery object to bind data to.
         * @param cookie The Cookie object containing the data.
         */
        static void bindForUpdate(QSqlQuery& query, const Cookie& cookie);

        /**
         * @brief Creates a Cookie object from a QSqlRecord.
         * @param record The QSqlRecord containing the cookie data.
         * @return A Cookie object populated with the data from the record.
         */
        static Cookie fromRecord(const QSqlRecord& record);

        /**
         * @brief Checks if a cookie matches an example cookie.
         * @param item The cookie to check.
         * @param example The example cookie to match against.
         * @return True if the cookies match, false otherwise.
         */
        static bool matches(const Cookie& item, const Cookie& example) noexcept {
            return item.species.name == example.species.name &&
                   item.length.toTicks() == example.length.toTicks() &&
                   item.diameter.toTicks() == example.diameter.toTicks() &&
                   item.drying == example.drying &&
                   item.location == example.location;
        }
    };
}