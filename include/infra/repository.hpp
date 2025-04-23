#pragma once
#include <optional>
#include <vector>
#include <typeindex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>
#include <QObject>
#include <functional>

// STD output
#include <iostream>

#include "infra/connection.hpp"

#include "infra/mappers/log_mapper.hpp"
#include "infra/mappers/cookie_mapper.hpp"
#include "infra/mappers/live_edge_slab_mapper.hpp"
#include "infra/mappers/lumber_mapper.hpp"
#include "infra/mappers/firewood_mapper.hpp"
#include "infra/mappers/cutlist_mapper.hpp"

namespace woodworks::infra {

class RepositoryNotifier : public QObject {
    Q_OBJECT
public:
    static RepositoryNotifier& instance() { static RepositoryNotifier inst; return inst; }
signals:
    void repositoryChanged();
};

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

            // Create a repository without a connection
            static QtSqlRepository<T> spawn() {
                auto& db = woodworks::infra::DbConnection::instance();
                return QtSqlRepository<T>(db);
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

            int add(const T& item) {
                QSqlQuery q(db_);
                if (!q.prepare(T::insertSQL())) {
                    throw std::runtime_error("Failed to prepare insert statement" + q.lastError().text().toStdString());
                }
                T::bindForInsert(q, item);
                if (!q.exec()) {
                    throw std::runtime_error(std::string("Failed to insert item: ") + q.lastError().text().toStdString());
                }
                int id = q.lastInsertId().toInt();
                RepositoryNotifier::instance().repositoryChanged();
                return id;
            }

            void update(const T& item) {
                QSqlQuery q(db_);
                q.prepare(T::updateSQL());
                std::cout << "Updating item with id: " << item.id.id << std::endl;
                if (!q.exec()) {
                    throw std::runtime_error("Failed to prepare update statement" + q.lastError().text().toStdString());
                }
                T::bindForUpdate(q, item);
                if (!q.exec()) {
                    throw std::runtime_error("Failed to update item");
                }
                RepositoryNotifier::instance().repositoryChanged();
            }

            void remove(int id) {
                QSqlQuery q(db_);
                q.prepare(T::deleteSQL());
                q.bindValue(0, id);
                if (!q.exec()) {
                    throw std::runtime_error(std::string("Failed to delete item: ") + q.lastError().text().toStdString());
                }
                RepositoryNotifier::instance().repositoryChanged();
            }

            template<typename Predicate>
            std::vector<T> filter(Predicate pred) {
                auto all = list();
                std::vector<T> result;
                for (const auto& item : all) {
                    if (pred(item)) result.push_back(item);
                }
                return result;
            }

            std::vector<T> filterByExample(const T& example) {
                // Requires T::matches(const T&, const T&)
                return filter([&](const T& item) { return T::matches(item, example); });
            }

        private:
            QSqlDatabase& db_;

    };
}