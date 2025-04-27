/**
 * @file repository.hpp
 * @brief Provides a generic repository pattern for managing database operations.
 * 
 * This file defines the `QtSqlRepository` template class for performing CRUD operations
 * on database entities and the `RepositoryNotifier` class for signaling repository changes.
 */

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

/**
 * @class RepositoryNotifier
 * @brief Singleton class for notifying changes in the repository.
 */
class RepositoryNotifier : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Gets the singleton instance of the notifier.
     * @return The singleton instance of `RepositoryNotifier`.
     */
    static RepositoryNotifier& instance() { static RepositoryNotifier inst; return inst; }

signals:
    /**
     * @brief Signal emitted when the repository changes.
     */
    void repositoryChanged();
};

/**
 * @class QtSqlRepository
 * @brief Template class for managing database operations for a specific entity type.
 * 
 * @tparam T The type of the entity managed by the repository.
 */
template<typename T>
class QtSqlRepository {
public:
    /**
     * @brief Constructs a repository with a given database connection.
     * @param db The database connection to use.
     */
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

    /**
     * @brief Creates a repository using the default database connection.
     * @return A `QtSqlRepository` instance.
     */
    static QtSqlRepository<T> spawn() {
        auto& db = woodworks::infra::DbConnection::instance();
        return QtSqlRepository<T>(db);
    }

    /**
     * @brief Retrieves an entity by its ID.
     * @param id The ID of the entity to retrieve.
     * @return An optional containing the entity if found, or `std::nullopt` otherwise.
     */
    std::optional<T> get(int id) {
        QSqlQuery q(db_);
        q.prepare(T::selectOneSQL());
        q.bindValue(0, QVariant(id));
        if (!q.exec() || !q.next()) {
            return std::nullopt;
        }
        return T::fromRecord(q.record());
    }

    /**
     * @brief Retrieves all entities from the database.
     * @return A vector containing all entities.
     */
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

    /**
     * @brief Adds a new entity to the database.
     * @param item The entity to add.
     * @return The ID of the newly added entity.
     */
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

    /**
     * @brief Updates an existing entity in the database.
     * @param item The entity to update.
     */
    void update(const T& item) {
        QSqlQuery q(db_);
        if (!q.prepare(T::updateSQL())) {
            throw std::runtime_error("Failed to prepare update statement: " + q.lastError().text().toStdString());
        }
        std::cout << "Updating item with id: " << item.id.id << std::endl;
        T::bindForUpdate(q, item);
        if (!q.exec()) {
            throw std::runtime_error("Failed to update item: " + q.lastError().text().toStdString());
        }
        RepositoryNotifier::instance().repositoryChanged();
    }

    /**
     * @brief Removes an entity from the database by its ID.
     * @param id The ID of the entity to remove.
     */
    void remove(int id) {
        QSqlQuery q(db_);
        q.prepare(T::deleteSQL());
        q.bindValue(0, id);
        if (!q.exec()) {
            throw std::runtime_error(std::string("Failed to delete item: ") + q.lastError().text().toStdString());
        }
        RepositoryNotifier::instance().repositoryChanged();
    }

    /**
     * @brief Filters entities based on a predicate.
     * @tparam Predicate The type of the predicate function.
     * @param pred The predicate function to apply.
     * @return A vector of entities that match the predicate.
     */
    template<typename Predicate>
    std::vector<T> filter(Predicate pred) {
        auto all = list();
        std::vector<T> result;
        for (const auto& item : all) {
            if (pred(item)) result.push_back(item);
        }
        return result;
    }

    /**
     * @brief Filters entities based on an example entity.
     * @param example The example entity to match against.
     * @return A vector of entities that match the example.
     */
    std::vector<T> filterByExample(const T& example) {
        // Requires T::matches(const T&, const T&)
        return filter([&](const T& item) { return T::matches(item, example); });
    }

private:
    QSqlDatabase& db_; ///< The database connection used by the repository.
};

} // namespace woodworks::infra