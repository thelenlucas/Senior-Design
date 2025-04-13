// =====================================================================================================================
//  slab.hpp — Slab data‑model declaration (persistable + manufacturable)
// =====================================================================================================================
#pragma once
/**
 * @file slab.hpp
 * @brief Declaration of the Slab data‑model (persistable & manufacturable).
 *
 * Heavyweight definitions that require database access or complex logic live in *slabs.cpp*.
 */

#include <string>
#include <vector>
#include <optional>

#include "types.hpp"       // Drying enum, etc.
#include "logs.hpp"        // Log class (foreign‑key reference)
#include "interfaces.hpp"  // Persistent<>
#include "manufacturable.hpp"

// ---------------------------------------------------------------------------------------------------------------------
//  Slab – Persistable + Manufacturable board cut from a Log
// ---------------------------------------------------------------------------------------------------------------------

class Slab final : public Manufacturable<Slab>
{
public:
    // Construction ------------------------------------------------------------------
    Slab() = default;
    Slab(int                id,
         std::string        species,
         unsigned           thickness_eighths,
         unsigned           len_quarters,
         unsigned           width_eighths,
         Drying             drying          = Drying::WET,
         bool               smoothed        = false,
         std::string        location        = {},
         std::string        notes           = {});

    // Getters -----------------------------------------------------------------------
    [[nodiscard]] int                     get_id()          const noexcept override;
    [[nodiscard]] const std::string&      getSpecies()      const noexcept { return species_; }
    [[nodiscard]] unsigned                getThickness8()   const noexcept { return thickness_eighths_; }
    [[nodiscard]] unsigned                getLenQ()         const noexcept { return len_quarters_; }
    [[nodiscard]] unsigned                getWidth8()       const noexcept { return width_eighths_; }
    [[nodiscard]] Drying                  getDrying()       const noexcept { return drying_; }
    [[nodiscard]] bool                    isSmoothed()      const noexcept { return smoothed_; }
    [[nodiscard]] const std::string&      getLocation()     const noexcept { return location_; }
    [[nodiscard]] const std::string&      getNotes()        const noexcept { return notes_; }

    // Persistent interface ----------------------------------------------------------
    bool insert() override;                      // defined in slabs.cpp
    bool update() override;                      // defined in slabs.cpp
    static std::optional<Slab> get_by_id(int id);        // slabs.cpp
    static std::vector<Slab>   get_all();                // slabs.cpp

    // Manufacturable interface ------------------------------------------------------
    static std::vector<Slab> make_from_log(
        Log                     log,
        int                     len_quarters,
        std::optional<int>      thickness_eighths = std::nullopt,
        std::optional<int>      width_eighths     = std::nullopt,
        std::optional<Drying>   drying            = std::nullopt);   // slabs.cpp

    // Convenience factory used by SlabManufacturer (implemented in slabs.cpp)
    static std::vector<Slab> manufacture_and_persist_slabs(
        Log &                               log,
        const std::vector<struct InProgressSlab>& cuts,
        int                                 total_len_quarters,
        const std::string&                  location = {},
        const std::string&                  notes    = {});

private:
    int          id_{-1};
    std::string  species_;
    unsigned     thickness_eighths_{0};
    unsigned     len_quarters_{0};
    unsigned     width_eighths_{0};
    Drying       drying_{Drying::WET};
    bool         smoothed_{false};
    std::string  location_;
    std::string  notes_;
};