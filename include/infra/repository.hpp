#pragma once
#include <optional>
#include <vector>
#include <typeindex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>

namespace woodworks::infra {
    template<typename T>
    class QtSqlRepository {
        public:
            explicit QtSqlRepository(QSqlDatabase& db) : db_(db) {}

            std::optional<T> get(int id) {
                QSqlQuery q(db_);
                q.prepare(T::selectOneSQL());
                q.bindValue(0, QVariant(id));
                if (!q.exec() || !q.next()) {
                    std::nullopt;
                }
                return T::fromRecord(q.record());
            }

            std::vector<T> list() {
                QSqlQuery q(db_);
                q.prepare(T::selectAllSQL());
                if (!q.exec()) {
                    return {};
                }
                std::vector<T> result;
                while (q.next()) {
                    result.push_back(T::fromRecord(q.record()));
                }
                return result;
            }

            void add(const T& item) {
                QSqlQuery q(db_);
                q.prepare(T::insertSQL());
                T::bindForInsert(q, item);
                if (!q.exec()) {
                    throw std::runtime_error("Failed to insert item" + q.lastError().text().toStdString());
                }
            }

            void update(const T& item) {
                QSqlQuery q(db_);
                q.prepare(T::updateSQL());
                T::bindForUpdate(q, item);
                if (!q.exec()) {
                    throw std::runtime_error("Failed to update item");
                }
            }

            void remove(int id) {
                QSqlQuery q(db_);
                q.prepare(T::deleteSQL());
                q.bindValue(0, id);
                if (!q.exec()) {
                    throw std::runtime_error("Failed to delete item");
                }
            }

        private:
            QSqlDatabase& db_;

    };
}