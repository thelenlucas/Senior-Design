#include <QSqlQuery>
#include <QStringList>

#include "connection.hpp"

namespace woodworks::infra {
    // Fetches unique species present in the database
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

    // Unique drying options
    inline QStringList getUniqueDryingOptions() {
        auto &db = DbConnection::instance();
        QStringList dryingList;
        QSqlQuery query(db);
        if (!query.prepare(
            "SELECT DISTINCT drying FROM ("
            "SELECT drying FROM cookies UNION "
            "SELECT drying FROM firewood UNION "
            "SELECT drying FROM logs UNION "
            "SELECT drying FROM lumber UNION "
            "SELECT drying FROM live_edge_slabs)"
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

    // Gets the max of a column in a table
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

    // Gets the min of a column in a table
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

    // Gets all unique values of a column in a table
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