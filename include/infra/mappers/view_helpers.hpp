/**
 * @file view_helpers.hpp
 * @brief Provides helper functions and structures for creating and managing database views and filters.
 */

#pragma once
#include <QString>
#include <QStringList>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QVariant>
#include <variant>
#include <optional>
#include <iostream>
#include <iomanip>

/**
 * @namespace woodworks::infra
 * @brief Contains infrastructure-related classes and utilities.
 */
namespace woodworks::infra
{
    /**
     * @brief Creates an SQL statement for an individual view.
     * @param viewName The name of the view.
     * @param tableName The name of the table.
     * @param columns The columns to include in the view.
     * @return The SQL statement for creating the view.
     */
    inline QString makeIndividualViewSQL(const QString &viewName, const QString &tableName, const QStringList &columns)
    {
        QString selectCols = columns.join(", ");
        return QString("CREATE VIEW IF NOT EXISTS %1 AS SELECT %2 FROM %3")
            .arg(viewName)
            .arg(selectCols)
            .arg(tableName);
    }

    /**
     * @brief Creates an SQL statement for a grouped view.
     * @param viewName The name of the view.
     * @param tableName The name of the table.
     * @param selectExprs The expressions to select.
     * @param groupByExprs The expressions to group by.
     * @return The SQL statement for creating the grouped view.
     */
    inline QString makeGroupedViewSQL(const QString &viewName, const QString &tableName, const QStringList &selectExprs, const QStringList &groupByExprs)
    {
        QString selectCols = selectExprs.join(", ");
        QString groupBy = groupByExprs.join(", ");
        return QString("CREATE VIEW IF NOT EXISTS %1 AS SELECT %2 FROM %3 GROUP BY %4")
            .arg(viewName)
            .arg(selectCols)
            .arg(tableName)
            .arg(groupBy);
    }

    /**
     * @brief Creates a QSqlQueryModel for a given view.
     * @param viewName The name of the view.
     * @param parent The parent QObject.
     * @return A pointer to the QSqlQueryModel.
     */
    inline QSqlQueryModel *makeViewModel(const QString &viewName, QObject *parent)
    {
        QSqlQueryModel *model = new QSqlQueryModel(parent);
        model->setQuery(QString("SELECT * FROM %1").arg(viewName), QSqlDatabase::database());
        if (model->lastError().isValid())
        {
            qDebug() << "Query error:" << model->lastError().text();
        }
        return model;
    }

    /**
     * @struct Exact
     * @brief Represents an exact match filter rule.
     */
    struct Exact {
        QVariant value; ///< The value to match exactly.
    }; // `col = value`

    /**
     * @struct Numeric
     * @brief Represents a numeric filter rule with a minimum value.
     */
    struct Numeric {
        double minValue; ///< The minimum value for the filter.
    }; // `col >= minValue`

    /**
     * @struct EnumInc
     * @brief Represents an enumerated inclusion filter rule.
     */
    struct EnumInc {
        std::optional<int> chosen; ///< The chosen value, or none for any value.
    }; // none = ANY, some = `col = chosen`

    /**
     * @struct Max
     * @brief Represents a numeric filter rule with a maximum value.
     */
    struct Max {
        double maxValue; ///< The maximum value for the filter.
    }; // `col <= maxValue`

    /**
     * @struct Between
     * @brief Represents a numeric filter rule with a range.
     */
    struct Between {
        double minValue; ///< The minimum value for the range.
        double maxValue; ///< The maximum value for the range.
    }; // `col BETWEEN minValue AND maxValue`

    /**
     * @typedef FilterRule
     * @brief A variant type representing different filter rules.
     */
    using FilterRule = std::variant<Exact, Numeric, EnumInc, Max, Between>;

    /**
     * @struct FieldFilter
     * @brief Represents a filter applied to a database field.
     */
    struct FieldFilter {
        QString column; ///< The database column or view alias.
        FilterRule rule; ///< The filter rule to apply.

        /**
         * @brief Creates an exact match filter.
         * @param col The column name.
         * @param v The value to match.
         * @return A FieldFilter object.
         */
        inline FieldFilter exact(QString col, QVariant v)
        {
            return {std::move(col), Exact{v}};
        }

        /**
         * @brief Creates a minimum value filter.
         * @param col The column name.
         * @param v The minimum value.
         * @return A FieldFilter object.
         */
        inline FieldFilter min(QString col, double v)
        {
            return {std::move(col), Numeric{v}};
        }

        /**
         * @brief Creates a maximum value filter.
         * @param col The column name.
         * @param v The maximum value.
         * @return A FieldFilter object.
         */
        inline FieldFilter max(QString col, double v)
        {
            return {std::move(col), Max{v}};
        }

        /**
         * @brief Creates a range filter.
         * @param col The column name.
         * @param minV The minimum value.
         * @param maxV The maximum value.
         * @return A FieldFilter object.
         */
        inline FieldFilter between(QString col, double minV, double maxV)
        {
            return {std::move(col), Between{minV, maxV}};
        }

        /**
         * @brief Creates an enumerated inclusion filter for any value.
         * @param col The column name.
         * @return A FieldFilter object.
         */
        inline FieldFilter anyEnum(QString col)
        {
            return {std::move(col), EnumInc{std::nullopt}};
        }

        /**
         * @brief Creates an enumerated inclusion filter for a specific value.
         * @param col The column name.
         * @param e The enumerated value.
         * @return A FieldFilter object.
         */
        inline FieldFilter enumIs(QString col, int e)
        {
            return {std::move(col), EnumInc{e}};
        }
    };

    /**
     * @brief Creates a filtered QSqlQueryModel based on the provided filters.
     * @param tableOrView The table or view to query.
     * @param filters The filters to apply.
     * @param parent The parent QObject.
     * @return A pointer to the QSqlQueryModel.
     */
    inline QSqlQueryModel *makeFilteredModel(const QString &tableOrView, const QVector<FieldFilter> &filters, QObject *parent = nullptr)
    {
        // ‑‑ Build WHERE clause if we have filters
        QStringList clauses;
        clauses.reserve(filters.size());

        for (const FieldFilter &f : filters)
        {
            std::visit([&](auto &&r)
                       {
            using T = std::decay_t<decltype(r)>;

            if constexpr (std::is_same_v<T, Exact>) {
                // strings need quoting, others don’t
                if (r.value.type() == QVariant::String)
                    clauses << QString("%1 = '%2'").arg(f.column, r.value.toString());
                else
                    clauses << QString("%1 = %2").arg(f.column).arg(r.value.toString());
            }
            else if constexpr (std::is_same_v<T, Numeric>) {
                clauses << QString("%1 >= %2").arg(f.column).arg(r.minValue);
            }
            else if constexpr (std::is_same_v<T, EnumInc>) {
                if (r.chosen.has_value())          // only filter if they asked for a specific variant
                    clauses << QString("%1 = %2").arg(f.column).arg(*r.chosen);
            }
            else if constexpr (std::is_same_v<T, Max>) {
                clauses << QString("%1 <= %2").arg(f.column).arg(r.maxValue);
            }
            else if constexpr (std::is_same_v<T, Between>) {
                clauses << QString("%1 BETWEEN %2 AND %3").arg(f.column).arg(r.minValue).arg(r.maxValue);
            } }, f.rule);
        }

        QString where = clauses.isEmpty() ? QString()
                                          : QStringLiteral(" WHERE ") + clauses.join(" AND ");

        // Final query
        QString sql = QStringLiteral("SELECT * FROM %1%2").arg(tableOrView, where);

        std::cout << "[makeFilteredModel] SQL:" << sql.toStdString() << std::endl;

        auto *model = new QSqlQueryModel(parent);
        model->setQuery(sql, QSqlDatabase::database());

        if (model->lastError().isValid())
            qDebug() << "[makeFilteredModel] query error:" << model->lastError().text()
                     << "\nSQL:" << sql;

        return model;
    }
}