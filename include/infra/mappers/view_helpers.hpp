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

namespace woodworks::infra
{

    inline QString makeIndividualViewSQL(const QString &viewName, const QString &tableName, const QStringList &columns)
    {
        QString selectCols = columns.join(", ");
        return QString("CREATE VIEW IF NOT EXISTS %1 AS SELECT %2 FROM %3")
            .arg(viewName)
            .arg(selectCols)
            .arg(tableName);
    }

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

    // ──────────────────────────────────────────────────────────────
    //  Per‑field filter variant
    // ──────────────────────────────────────────────────────────────
    struct Exact
    {
        QVariant value;
    }; // `col = value`
    struct Numeric
    {
        double minValue;
    }; // `col >= minValue`
    struct EnumInc
    {
        std::optional<int> chosen;
    }; // none = ANY, some = `col = chosen`

    struct Max
    {
        double maxValue;
    }; // `col <= maxValue`
    struct Between
    {
        double minValue;
        double maxValue;
    }; // `col BETWEEN minValue AND maxValue`

    using FilterRule = std::variant<Exact, Numeric, EnumInc, Max, Between>;

    struct FieldFilter
    {
        QString column; // DB column or view alias
        FilterRule rule;

        inline FieldFilter exact(QString col, QVariant v)
        {
            return {std::move(col), Exact{v}};
        }

        inline FieldFilter min(QString col, double v)
        {
            return {std::move(col), Numeric{v}};
        }

        inline FieldFilter max(QString col, double v)
        {
            return {std::move(col), Max{v}};
        }

        inline FieldFilter between(QString col, double minV, double maxV)
        {
            return {std::move(col), Between{minV, maxV}};
        }

        inline FieldFilter anyEnum(QString col)
        {
            return {std::move(col), EnumInc{std::nullopt}};
        }

        inline FieldFilter enumIs(QString col, int e)
        {
            return {std::move(col), EnumInc{e}};
        }
    };

    // ──────────────────────────────────────────────────────────────
    //  Single helper that understands the variant
    // ──────────────────────────────────────────────────────────────
    inline QSqlQueryModel *makeFilteredModel(
        const QString &tableOrView,
        const QVector<FieldFilter> &filters,
        QObject *parent = nullptr)
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