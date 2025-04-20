#pragma once
#include <QString>
#include <QStringList>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

namespace woodworks::infra {

inline QString makeIndividualViewSQL(const QString& viewName, const QString& tableName, const QStringList& columns) {
    QString selectCols = columns.join(", ");
    return QString("CREATE VIEW IF NOT EXISTS %1 AS SELECT %2 FROM %3")
            .arg(viewName).arg(selectCols).arg(tableName);
}

inline QString makeGroupedViewSQL(const QString& viewName, const QString& tableName, const QStringList& selectExprs, const QStringList& groupByExprs) {
    QString selectCols = selectExprs.join(", ");
    QString groupBy = groupByExprs.join(", ");
    return QString("CREATE VIEW IF NOT EXISTS %1 AS SELECT %2 FROM %3 GROUP BY %4")
            .arg(viewName).arg(selectCols).arg(tableName).arg(groupBy);
}

inline QSqlQueryModel* makeViewModel(const QString& viewName, QObject* parent) {
    QSqlQueryModel* model = new QSqlQueryModel(parent);
    model->setQuery(QString("SELECT * FROM %1").arg(viewName), QSqlDatabase::database());
    if (model->lastError().isValid()) {
        qDebug() << "Query error:" << model->lastError().text();
    }
    return model;
}

} // namespace woodworks::infra::mappers
