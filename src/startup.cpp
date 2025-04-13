#include "schema.hpp"
#include <QSqlQuery>
#include <QSqlError>

bool ensureSchema(QSqlDatabase& db)
{
    QSqlQuery q{db};

    // 1.  Check a user_version PRAGMA so we know whether to rebuild.
    q.exec("PRAGMA user_version");
    if (!q.next()) return false;
    const int current = q.value(0).toInt();
    if (current >= schema::kSchemaVersion) return true;

    // 2.  (Re‑)create the views inside a transaction.
    db.transaction();
    for (const auto& viewSQL : schema::kCreateViews) {
        if (!q.exec(viewSQL)) {
            qDebug() << "Failed to create view:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }
    // 3.  Store the new version.
    q.exec(QStringLiteral("PRAGMA user_version = %1").arg(schema::kSchemaVersion));
    return db.commit();
}
