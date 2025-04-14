#pragma once
/**
 * @file logs.hpp
 * @brief Defines the Log class representing a log of wood, including its properties and persistence logic.
 * @details Doxygen comments from the original file are preserved. Implementation details that touch the
 *          database or heavyweight business logic live in *logs.cpp*; this header remains lightweight
 *          to maximise compiler inlining opportunities.
 */

#include <string>
#include <vector>
#include <optional>
#include <iostream>   // For optional debug logging

#include "types.hpp"        // Drying enum
#include "logs.hpp"         // Forward declarations for related helpers (if any)
#include "interfaces.hpp"   // Persistent<>

/**
 * @brief Controls verbose logging specific to Log class operations. Set to false to disable.
 */
#define LOGS_LOGGING true

// ---------------------------------------------------------------------------------------------------------------------
//  Log – Persistable model of a raw log ready for milling
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Represents a single log of wood, tracking its properties, cost, and status.
 * @details This class manages log data and interacts with the database for persistence,
 *          inheriting from the Persistent<Log> interface.
 */
class Log final : public Persistent<Log>
{
public:
    // Construction ------------------------------------------------------------------
    Log() = default;
    Log(int                id,
        std::string        species,
        unsigned           len_quarters,
        unsigned           diameter_quarters,
        unsigned           cost_cents_quarters,
        unsigned           quality,
        Drying             drying,
        std::string        location = {},
        std::string        notes    = {});

    // Getters -----------------------------------------------------------------------
    [[nodiscard]] int                get_id()            const noexcept override { return id_; }
    [[nodiscard]] const std::string& getSpecies()        const noexcept { return species_; }
    [[nodiscard]] unsigned           getLenQuarters()    const noexcept { return len_quarters_; }
    [[nodiscard]] unsigned           getDiameterQuarters() const noexcept { return diameter_quarters_; }
    [[nodiscard]] unsigned           getCostCentsQuarters() const noexcept { return cost_cents_quarters_; }
    [[nodiscard]] unsigned           getQuality()        const noexcept { return quality_; }
    [[nodiscard]] const std::string& getLocation()       const noexcept { return location_; }
    [[nodiscard]] const std::string& getNotes()          const noexcept { return notes_; }
    [[nodiscard]] Drying             getDrying()         const noexcept { return drying_; }

    /**
     * @brief Calculates the remaining usable length of the log in quarters of an inch.
     * @details This subtracts the total length of all items already manufactured
     *          (e.g., slabs, cookies) from this log's original length (`len_quarters_`).
     *          It queries the `taken_len_all` database view to get the used length.
     * @return The currently available length in quarters of an inch. Returns 0 if the
     *         calculated available length is negative (which shouldn't happen ideally).
     */
    unsigned getAvailableLength() const;

    // Workflow helpers --------------------------------------------------------------
    void scrap();                               ///< Marks the log as scrapped / unusable.
    unsigned multiCut(unsigned amt_quarters, std::string type); /// Creates a cut for multiple items, returns the partial_cut id

    // Persistent interface ----------------------------------------------------------
    bool insert() override;                     // defined in logs.cpp
    bool update() override;                     // defined in logs.cpp
    static std::optional<Log> get_by_id(int id);          // logs.cpp
    static std::vector<Log>   get_all();                  // logs.cpp

private:
    int          id_{-1};
    std::string  species_;
    unsigned     len_quarters_{0};
    unsigned     diameter_quarters_{0};
    unsigned     cost_cents_quarters_{0};
    unsigned     quality_{0};
    std::string  location_;
    std::string  notes_;
    Drying       drying_{Drying::WET};
};