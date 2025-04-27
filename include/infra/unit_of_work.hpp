/**
 * @file unit_of_work.hpp
 * @brief Provides an RAII transaction guard for Qt/SQLite.
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

#pragma once
#include <QSqlDatabase>

/**
 * @namespace woodworks::infra
 * @brief Contains infrastructure-related classes and utilities.
 */
namespace woodworks::infra {

/**
 * @class UnitOfWork
 * @brief Manages a database transaction using RAII.
 */
class UnitOfWork {
public:
    /**
     * @brief Constructs a UnitOfWork and begins a transaction.
     * @param db The database connection.
     */
    explicit UnitOfWork(QSqlDatabase& db);

    /**
     * @brief Destroys the UnitOfWork and rolls back the transaction if not committed.
     */
    ~UnitOfWork();

    /**
     * @brief Commits the transaction.
     */
    void commit();

    // No copy or move
    UnitOfWork(const UnitOfWork&) = delete;
    UnitOfWork& operator=(const UnitOfWork&) = delete;

private:
    /**
     * @var UnitOfWork::db_
     * @brief The database connection used by the UnitOfWork.
     */
    QSqlDatabase& db_;

    /**
     * @var UnitOfWork::committed_
     * @brief Indicates whether the transaction has been committed.
     */
    bool committed_{false};
};
}