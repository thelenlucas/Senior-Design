/**
 * @file live_edge_slab.hpp
 * @brief Defines the LiveEdgeSlab struct for live-edge slabs and database mapping utilities.
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
     * @struct LiveEdgeSlab
     * @brief Represents a live edge slab with attributes such as species, dimensions, drying state, surfacing, worth, location, notes, and image data.
     */
    struct LiveEdgeSlab {
        /** @brief Unique identifier for the slab. */
        Id id{-1};
        /** @brief Species of the slab. */
        Species species{""};
        /** @brief Length of the slab. */
        Length length{Length::fromTicks(0)};
        /** @brief Width of the slab. */
        Length width{Length::fromTicks(0)};
        /** @brief Thickness of the slab. */
        Length thickness{Length::fromTicks(0)};
        /** @brief Drying state of the slab. */
        Drying drying{Drying::GREEN};
        /** @brief Surfacing state of the slab. */
        SlabSurfacing surfacing{SlabSurfacing::RGH};
        /** @brief Monetary worth of the slab. */
        Dollar worth{0};
        /** @brief Storage location of the slab. */
        std::string location{""};
        /** @brief Additional notes about the slab. */
        std::string notes{""};
        /** @brief Image data buffer for the slab. */
        QByteArray imageBuffer{};

        /**
         * @brief Creates an uninitialized LiveEdgeSlab object with default values.
         * @return An uninitialized LiveEdgeSlab.
         */
        static LiveEdgeSlab uninitialized() {
            LiveEdgeSlab slab;
            slab.id = Id{-1};
            slab.species = Species{""};
            slab.length = Length::fromTicks(0);
            slab.width = Length::fromTicks(0);
            slab.thickness = Length::fromTicks(0);
            slab.drying = Drying::GREEN;
            slab.surfacing = SlabSurfacing::RGH;
            slab.worth = Dollar{0};
            slab.location = "";
            slab.notes = "";
            slab.imageBuffer = QByteArray();
            return slab;
        }

        /**
         * @brief Converts the slab to a sales::Product representation.
         * @return A sales::Product representing the slab.
         */
        sales::Product toProduct();

        // ---- Mapping -----
        /** @brief SQL for creating the slabs table. */
        static QString createDbSQL();
        /** @brief SQL for inserting a slab record. */
        static QString insertSQL();
        /** @brief SQL view for individual slab entries. */
        static QString individualViewSQL();
        /** @brief SQL view for grouped slab entries. */
        static QString groupedViewSQL();
        /** @brief SQL for updating a slab record. */
        static QString updateSQL();
        /** @brief SQL for selecting one slab record. */
        static QString selectOneSQL();
        /** @brief SQL for selecting all slab records. */
        static QString selectAllSQL();
        /** @brief SQL for deleting a slab record. */
        static QString deleteSQL();

        /**
         * @brief Binds slab data to a QSqlQuery for insertion.
         * @param query The QSqlQuery to bind to.
         * @param slab The LiveEdgeSlab instance.
         */
        static void bindForInsert(QSqlQuery&, const LiveEdgeSlab&);
        /**
         * @brief Binds slab data to a QSqlQuery for updating.
         * @param query The QSqlQuery to bind to.
         * @param slab The LiveEdgeSlab instance.
         */
        static void bindForUpdate(QSqlQuery&, const LiveEdgeSlab&);
        /**
         * @brief Constructs a LiveEdgeSlab from a QSqlRecord.
         * @param record The record containing slab data.
         * @return Populated LiveEdgeSlab.
         */
        static LiveEdgeSlab fromRecord(const QSqlRecord&);

        /**
         * @brief Checks if two slabs match on key fields.
         * @param item The slab to compare.
         * @param example The example slab to match against.
         * @return True if matching, false otherwise.
         */
        static bool matches(const LiveEdgeSlab& item, const LiveEdgeSlab& example) noexcept {
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