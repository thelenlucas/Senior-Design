#pragma once

#include <QSqlDatabase>
#include <mutex>

namespace woodworks::infra {
    class DbConnection {
    public:
        static QSqlDatabase& instance();

        QSqlDatabase& db() {
            return db_;
        }

    private:
        static inline QSqlDatabase db_;
        static inline std::once_flag initFlag_;
    };
}