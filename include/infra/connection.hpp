/**
 * @file connection.hpp
 * @brief Provides a singleton database connection for the application.
 */

#pragma once

#include <QSqlDatabase>
#include <mutex>

/**
 * @namespace woodworks::infra
 * @brief Contains infrastructure-related classes and utilities.
 */
namespace woodworks::infra {

    /**
     * @class DbConnection
     * @brief Manages a singleton database connection.
     */
    class DbConnection {
    public:
        /**
         * @brief Retrieves the singleton instance of the database connection.
         * @return A reference to the QSqlDatabase instance.
         */
        static QSqlDatabase& instance();

        /**
         * @brief Provides access to the database connection.
         * @return A reference to the QSqlDatabase instance.
         */
        QSqlDatabase& db() {
            return db_;
        }

    private:
        /**
         * @var DbConnection::db_
         * @brief The singleton database connection instance.
         */
        static inline QSqlDatabase db_;

        /**
         * @var DbConnection::initFlag_
         * @brief Ensures the database connection is initialized only once.
         */
        static inline std::once_flag initFlag_;
    };
}