#include <QSqlError>
#include <QSqlQuery>
#include <mutex>

#include "infra/connection.hpp"

namespace woodworks::infra
{
    QSqlDatabase &DbConnection::instance()
    {
        std::call_once(initFlag_, []
                       {
            db_ = QSqlDatabase::addDatabase("QSQLITE");
            db_.setDatabaseName("woodworks.db");
            if (!db_.open()) {
                throw std::runtime_error("Failed to open database" + db_.lastError().text().toStdString());
            }
            db_.exec("PRAGMA foreign_keys = ON;"); });
        return db_;
    }
}