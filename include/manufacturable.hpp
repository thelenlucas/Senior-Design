/**
 * @file manufacturable.hpp
 * @brief Defines the interface for items that can be manufactured from Log objects and persisted.
 */
#ifndef MANUFACTURABLE_HPP
#define MANUFACTURABLE_HPP

#include <vector>
#include <optional>
#include "interfaces.hpp" // Provides Persistent
#include "types.hpp"      // Provides Drying enum
#include "domain/types.hpp"

// Forward declaration if Log is only used by pointer/reference here,
// but #include is likely needed for Derived::make_from_log implementations.

/**
 * @brief Interface for classes representing items that can be manufactured from a Log.
 * @details This interface uses CRTP and inherits from Persistent<Derived>, meaning
 *          manufacturable items are also expected to be persistable. Derived classes
 *          must implement the static `make_from_log` method with the specific
 *          manufacturing logic.
 * @tparam Derived The derived class implementing this interface (e.g., `class Slab : public Manufacturable<Slab>`).
 */
template <typename Derived>
class Manufacturable : public Persistent<Derived> {
public:
    /**
     * @brief Default virtual destructor.
     */
    virtual ~Manufacturable() = default;

    /**
     * @brief Creates one or more items of the Derived type from a source Log object and persists them.
     * @details This static method delegates the actual object creation logic to the
     *          Derived class's static `make_from_log` implementation and then calls
     *          `insert()` on each newly created object before returning them.
     *
     * @param log The source Log object to manufacture from.
     * @param len_quarters The desired length of the manufactured item(s) in quarters of an inch.
     * @param thickness_quarters Optional desired thickness in quarters of an inch. Interpretation depends on the Derived type.
     * @param width_quarters Optional desired width in quarters of an inch. Interpretation depends on the Derived type.
     * @param drying Optional desired drying status for the manufactured item(s).
     * @return A std::vector containing the newly manufactured and persisted objects of the Derived type.
     */
    static std::vector<Derived> make_from_log(
        Log log,
        int len_quarters,
        std::optional<int> thickness_quarters = std::nullopt,
        std::optional<int> width_quarters = std::nullopt,
        std::optional<woodworks::domain::types::Drying> drying = std::nullopt
    ) {
        // Delegate creation to the derived class's specific implementation
        std::vector<Derived> products = Derived::make_from_log(log, len_quarters, thickness_quarters, width_quarters, drying);
        // Persist each newly created product
        for (Derived &product : products) {
            product.insert(); // Relies on Persistent interface implementation
        }
        return products;
    }
};

#endif // MANUFACTURABLE_HPP
