#ifndef INTERFACES_HPP
#define INTERFACES_HPP
#include <optional>
#include "types.hpp"
#include <vector>

/// @brief  Interface for classes that can be persisted to a database
/// @tparam Derived The derived class that implements this interface
template <typename Derived>
class Persistent {
public:
    virtual ~Persistent() = default;
    /// @brief Get the ID of the object
    /// @return The ID of the object
    virtual int get_id() const = 0;
    /// @brief Insert the object in the database
    /// @return True if the operation was successful, false otherwise
    virtual bool insert() = 0;
    /// @brief Update the object in the database
    /// @return True if the operation was successful, false otherwise
    virtual bool update() = 0;

    /// @brief  Get an object by its ID
    /// @param  id The ID of the object to get
    /// @return An optional containing the object if it exists, or an empty optional otherwise
    static std::optional<Derived> get_by_id(int id) {
        return Derived::get_by_id(id); // Ensures every Derived class implements this
    }

    /// @brief  Get all objects of this type
    /// @return A vector containing all objects of this type
    static std::vector<Derived> get_all() {
        return Derived::get_all(); // Ensures every Derived class implements this
    }
};
#endif // INTERFACES_HPP