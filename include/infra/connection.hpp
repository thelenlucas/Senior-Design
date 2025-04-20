#pragma once

#include <QSqlDatabase>
#include <mutex>

namespace woodworks::infra {
    class DbConnection {
    public:
        static QSqlDatabase& instance();

    private:
        static inline QSqlDatabase db_;
        static inline std::once_flag initFlag_;
    };
}