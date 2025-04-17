#pragma once
/*
 * @file lumber.hpp
 * @brief Declarations for Lumber data model
 */

#include <string>
#include <vector>
#include <optional>
#include "types.hpp"
#include "logs.hpp"

#include "interfaces.hpp"

/*
 * @brief Controls verbose logging specific to Lumber class operations. Set to false to disable.
 */
#define LUMBER_LOGGING true

// ---------------------------------------------------------------------------------------------------------------------
//  Log – Persistable model of a piece of dimensional lumber
// ---------------------------------------------------------------------------------------------------------------------
class Lumber final : public Persistent<Lumber>
{
public:
    // Construction ------------------------------------------------------------------
    Lumber() = default;
    Lumber(std::string        species,
           unsigned           thickness_eighths,
           unsigned           len_quarters,
           unsigned           width_eighths,
           Drying             drying          = Drying::WET,
           bool               smoothed        = false,
           std::string        location        = {},
           std::string        notes           = {});

    // Getters -----------------------------------------------------------------------
    [[nodiscard]] int                get_id()          const noexcept override { return id_; };
    [[nodiscard]] const std::string& getSpecies()      const noexcept { return species_; }
    [[nodiscard]] unsigned           getThickness4()   const noexcept { return thickness_quarters_; }
    [[nodiscard]] unsigned           getLenQ()         const noexcept { return len_quarters_; }
    [[nodiscard]] unsigned           getWidth4()       const noexcept { return width_quarters_; }
    [[nodiscard]] Drying             getDrying()       const noexcept { return drying_; }
    [[nodiscard]] unsigned           isSmoothed()      const noexcept { return smoothed_; }
    [[nodiscard]] const std::string& getLocation()     const noexcept { return location_; }
    [[nodiscard]] const std::string& getNotes()        const noexcept { return notes_; }

    // Persistent interface ----------------------------------------------------------
    bool insert() override;                      // defined in lumber.cpp
    bool update() override;                      // defined in lumber.cpp
    static std::optional<Lumber> get_by_id(int id);  // defined in lumber.cpp
    static std::vector<Lumber>   get_all();          // defined in lumber.cpp


private:
    int          id_{-1};
    std::string  species_;
    unsigned     thickness_quarters_{0};
    unsigned     len_quarters_{0};
    unsigned     width_quarters_{0};
    Drying       drying_{Drying::WET};
    unsigned     smoothed_{0};
    std::string  location_;
    std::string  notes_;
};