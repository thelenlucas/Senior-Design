/**
 * @file helpers.hpp
 * @brief Provides utility functions for querying unique values and column statistics from the database.
 */

#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>

#include "connection.hpp"

/**
 * @namespace woodworks::infra
 * @brief Contains infrastructure-related classes and utilities.
 */
namespace woodworks::infra {

    /**
     * @brief Fetches unique species present in the database.
     * @return A QStringList of unique species.
     */
    inline QStringList getUniqueSpecies() {
        auto &db = DbConnection::instance();

        // From species in [cookies, firewood, logs, lumber, slabs]
        QStringList speciesList;
        QSqlQuery query(db);
        if (!query.prepare(
            "SELECT DISTINCT species FROM ("
            "SELECT species FROM cookies UNION "
            "SELECT species FROM firewood UNION "
            "SELECT species FROM logs UNION "
            "SELECT species FROM lumber UNION "
            "SELECT species FROM live_edge_slabs)"
        )) {
            qDebug() << "Error preparing query for unique species:" << query.lastError().text();
            return speciesList;
        }


        auto out = QStringList();
        if (query.exec()) {
            while (query.next()) {
                out << query.value(0).toString();
            }
        } else {
            qDebug() << "Error fetching unique species:" << query.lastError().text();
        }
        return out;
    }

    /**
     * @brief Fetches unique drying options present in the database.
     * @return A QStringList of unique drying options.
     */
    inline QStringList getUniqueDryingOptions() {
        auto &db = DbConnection::instance();
        QStringList dryingList;
        QSqlQuery query(db);
        if (!query.prepare(
            "SELECT DISTINCT Drying FROM ("
            "SELECT Drying FROM display_cookies UNION "
            "SELECT Drying FROM display_firewood UNION "
            "SELECT Drying FROM display_logs UNION "
            "SELECT Drying FROM display_lumber UNION "
            "SELECT Drying FROM display_slabs)"
        )) {
            qDebug() << "Error preparing query for unique drying options:" << query.lastError().text();
            return dryingList;
        }

        auto out = QStringList();
        if (query.exec()) {
            while (query.next()) {
                out << query.value(0).toString();
            }
        } else {
            qDebug() << "Error fetching unique drying options:" << query.lastError().text();
        }
        return out;
    }

    /**
     * @brief Fetches unique locations present in the database.
     * @return A QStringList of unique locations.
     */
    inline QStringList getUniqueLocations() {
        auto &db = DbConnection::instance();
        QStringList locationList;
        QSqlQuery query(db);
        if (!query.prepare(
            "SELECT DISTINCT location FROM ("
            "SELECT location FROM cookies UNION "
            "SELECT location FROM firewood UNION "
            "SELECT location FROM logs UNION "
            "SELECT location FROM lumber UNION "
            "SELECT location FROM live_edge_slabs)"
        )) {
            qDebug() << "Error preparing query for unique locations:" << query.lastError().text();
            return locationList;
        }

        auto out = QStringList();
        if (query.exec()) {
            while (query.next()) {
                out << query.value(0).toString();
            }
        } else {
            qDebug() << "Error fetching unique locations:" << query.lastError().text();
        }
        return out;
    }

    /**
     * @brief Gets the maximum value of a column in a table.
     * @param tableName The name of the table.
     * @param columnName The name of the column.
     * @return The maximum value of the column, or -1 if an error occurs.
     */
    inline int getMaxOfColumn(const QString &tableName, const QString &columnName) {
        auto &db = DbConnection::instance();
        QSqlQuery query(db);
        query.prepare(QString("SELECT MAX(%1) FROM %2").arg(columnName, tableName));
        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            return -1;
        }
        if (query.next()) {
            return query.value(0).toInt();
        }
        return -1;
    }

    /**
     * @brief Gets the minimum value of a column in a table.
     * @param tableName The name of the table.
     * @param columnName The name of the column.
     * @return The minimum value of the column, or -1 if an error occurs.
     */
    inline int getMinOfColumn(const QString &tableName, const QString &columnName) {
        auto &db = DbConnection::instance();
        QSqlQuery query(db);
        query.prepare(QString("SELECT MIN(%1) FROM %2").arg(columnName, tableName));
        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            return -1;
        }
        if (query.next()) {
            return query.value(0).toInt();
        }
        return -1;
    }

    /**
     * @brief Gets all unique values of a column in a table.
     * @param tableName The name of the table.
     * @param columnName The name of the column.
     * @return A QStringList of unique values in the column.
     */
    inline QStringList getUniqueValuesOfColumn(const QString &tableName, const QString &columnName) {
        auto &db = DbConnection::instance();
        QSqlQuery query(db);
        query.prepare(QString("SELECT DISTINCT %1 FROM %2").arg(columnName, tableName));
        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            return QStringList();
        }
        QStringList values;
        while (query.next()) {
            values << query.value(0).toString();
        }
        return values;
    }
}