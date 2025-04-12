/**
 * @file interfaces.hpp
 * @brief Defines common interfaces used throughout the application, such as persistence.
 */
#ifndef INTERFACES_HPP
#define INTERFACES_HPP

#include <optional>
#include "types.hpp" // Assuming types.hpp defines necessary types, otherwise remove/change
#include <vector>

/**
 * @brief Interface for classes that can be persisted to and retrieved from a database.
 * @details This uses the Curiously Recurring Template Pattern (CRTP). Derived classes
 *          must implement the static methods get_by_id and get_all.
 * @tparam Derived The derived class that implements this interface (e.g., `class MyData : public Persistent<MyData>`).
 */
template <typename Derived>
class Persistent {
public:
    /**
     * @brief Default virtual destructor.
     */
    virtual ~Persistent() = default;

    /**
     * @brief Get the unique identifier of the object.
     * @return The integer ID of the object.
     */
    virtual int get_id() const = 0;

    /**
     * @brief Insert the current state of the object into the database as a new entry.
     * @return True if the insertion was successful, false otherwise.
     */
    virtual bool insert() = 0;

    /**
     * @brief Update the existing database entry corresponding to this object's ID
     *        with the current state of the object.
     * @return True if the update was successful, false otherwise.
     */
    virtual bool update() = 0;

    /**
     * @brief Retrieve an object from the database by its unique identifier.
     * @param id The ID of the object to retrieve.
     * @return An std::optional containing the object if found, or std::nullopt otherwise.
     */
    static std::optional<Derived> get_by_id(int id) {
        // Static methods in the base class template delegate to the derived class's static methods.
        return Derived::get_by_id(id);
    }

    /**
     * @brief Retrieve all objects of this type from the database.
     * @return A std::vector containing all objects of the derived type.
     */
    static std::vector<Derived> get_all() {
        // Static methods in the base class template delegate to the derived class's static methods.
        return Derived::get_all();
    }
};
#endif // INTERFACES_HPP
