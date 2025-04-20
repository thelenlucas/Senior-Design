#pragma once
#include <optional>
#include <vector>
#include <typeindex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>

// STD output
#include <iostream>

#include "infra/mappers/log_mapper.hpp"
#include "infra/mappers/cookie_mapper.hpp"
#include "infra/mappers/live_edge_slab_mapper.hpp"
#include "infra/mappers/lumber_mapper.hpp"

namespace woodworks::infra {
    template<typename T>
    class QtSqlRepository {
        public:
            explicit QtSqlRepository(QSqlDatabase& db) : db_(db) {
                // Create the repo if it does not exist
                std::cout << "Creating repository for " << typeid(T).name() << std::endl;
                QSqlQuery q(db_);
                q.prepare(T::createDbSQL());
                if (!q.exec()) {
                    throw std::runtime_error("Failed to create table: " + q.lastError().text().toStdString());
                }
                q.prepare(T::individualViewSQL());
                if (!q.exec()) {
                    throw std::runtime_error("Failed to create view: " + q.lastError().text().toStdString());
                }
                q.prepare(T::groupedViewSQL());
                if (!q.exec()) {
                    throw std::runtime_error("Failed to create view: " + q.lastError().text().toStdString());
                }
            }

            std::optional<T> get(int id) {
                QSqlQuery q(db_);
                q.prepare(T::selectOneSQL());
                q.bindValue(0, QVariant(id));
                if (!q.exec() || !q.next()) {
                    return std::nullopt;
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
                if (!q.prepare(T::insertSQL())) {
                    throw std::runtime_error("Failed to prepare insert statement" + q.lastError().text().toStdString());
                }
                T::bindForInsert(q, item);
                if (!q.exec()) {
                    throw std::runtime_error(std::string("Failed to insert item: ") + q.lastError().text().toStdString());
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
                q.bindValue(":id", id);
                if (!q.exec()) {
                    throw std::runtime_error(std::string("Failed to delete item: ") + q.lastError().text().toStdString());
                }
            }

        private:
            QSqlDatabase& db_;

    };
}