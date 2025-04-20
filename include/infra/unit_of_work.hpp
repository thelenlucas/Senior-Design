#pragma once
#include <QSqlDatabase>

/**
 * @file  unit_of_work.hpp
 * @brief RAII transaction guard for Qt/SQLite.
 *
 * Usage:
 *   auto& db = DbConnection::instance();
 *   {
 *       UnitOfWork uow(db);
 *       repo.add(obj);
 *       …
 *       uow.commit();   // or omit => automatic rollback
 *   }
 */

namespace woodworks::infra {
    class UnitOfWork {
    public:
        explicit UnitOfWork(QSqlDatabase& db);
        ~UnitOfWork();
        void commit();

        // No copy or move
        UnitOfWork(const UnitOfWork&) = delete;
        UnitOfWork& operator=(const UnitOfWork&) = delete;

    private:
        QSqlDatabase& db_;
        bool committed_{false};
    };
}