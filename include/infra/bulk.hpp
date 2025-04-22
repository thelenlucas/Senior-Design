#pragma once

#include "infra/repository.hpp"
#include <vector>
#include <algorithm>

namespace woodworks::infra {

template<typename T>
class Bulk {
public:
    explicit Bulk(QtSqlRepository<T>& repo)
        : repo_(repo), items_(repo.list()) {}

    template<typename Predicate>
    Bulk(QtSqlRepository<T>& repo, Predicate pred)
        : repo_(repo), items_(repo.filter(pred)) {}

    Bulk(QtSqlRepository<T>& repo, const T& example)
        : repo_(repo), items_(repo.filterByExample(example)) {}

    std::vector<T>& items() { return items_; }
    const std::vector<T>& items() const { return items_; }

    template<typename Func>
    void forEach(Func func) {
        for (auto& item : items_) func(item);
    }

    void commitUpdates() {
        for (auto& item : items_) repo_.update(item);
    }

    void removeAll() {
        for (auto& item : items_) repo_.remove(item.id.id);
        items_.clear();
    }

private:
    QtSqlRepository<T>& repo_;
    std::vector<T> items_;
};

} // namespace woodworks::infra
