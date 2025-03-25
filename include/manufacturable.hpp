#ifndef MANUFACTURABLE_HPP
#define MANUFACTURABLE_HPP

#include <vector>
#include <optional>
#include "interfaces.hpp"
#include "types.hpp"
#include "logs.hpp"

/// @brief  Interface for classes that can be manufactured
/// @tparam Derived The derived class that implements this interface
template <typename Derived>
class Manufacturable : public Persistent<Derived> {
public:
    virtual ~Manufacturable() = default;

    /// @brief  Make one or more objects from a log length
    /// @param  log The log to make the object from
    /// @param  len The length of the object, always required
    /// @param  thickness The thickness of the object, optional for some types (e.g cookies)
    /// @param  width The width of the object, optional for some types (e.g live-edge slabs)
    /// @param  drying The type of drying the object will undergo, optional
    static std::vector<Derived> make_from_log(
        Log log, 
        int len_quarters,
        std::optional<int> thickness_quarters = std::nullopt,
        std::optional<int> width_quarters = std::nullopt,
        std::optional<Drying> drying = std::nullopt
    ) {
        std::vector<Derived> products = Derived::make_from_log(log, len_quarters, thickness_quarters, width_quarters, drying); // Must be implemented by every Derived class
        for (Derived &product : products) {
            product.insert();
        }
        return products;
    }
};

#endif