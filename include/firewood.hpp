#pragma once
/**
 * @file firewood.hpp
 * @brief Declarations for Firewood data‑model and related helpers.
 *
 * Heavyweight definitions (database access, manufacturing logic, etc.) live in *firewood.cpp*.
 * All code in this header is intentionally lightweight to maximise inlining.
 */

#include <string>
#include <vector>
#include <optional>
#include "types.hpp"        // Drying, etc.
#include "logs.hpp"         // Log class
#include "interfaces.hpp"   // Persistent<>
#include "manufacturable.hpp"

#define FIREWOOD_LOGGING true

// ---------------------------------------------------------------------------------------------------------------------
//  Firewood – Persistable + Manufacturable product cut from a Log
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @class Firewood
 * @brief Represents a piece of firewood that can be manufactured from logs.
 * 
 * The Firewood class provides an interface for managing firewood objects, 
 * including their properties, database persistence, and manufacturing from logs.
 * It is a final class that inherits from Manufacturable<Firewood>.
 */
class Firewood final : public Manufacturable<Firewood>
{
public:
    /**
     * @brief Default constructor for Firewood.
     */
    Firewood() = default;

    /**
     * @brief Constructs a Firewood object with the specified properties.
     * 
     * @param id The unique identifier for the firewood.
     * @param species The species of the wood.
     * @param drying The drying state of the wood (e.g., wet or dry).
     * @param cubic_feet The volume of the firewood in cubic feet.
     * @param taken_len_quarters The length of the log already consumed, in quarters.
     * @param location The location where the firewood is stored (optional).
     * @param notes Additional notes about the firewood (optional).
     */
    Firewood(int id,
             std::string species,
             Drying drying,
             unsigned cubic_feet,
             unsigned taken_len_quarters,
             std::string location = {},
             std::string notes = {});

    /**
     * @brief Gets the unique identifier of the firewood.
     * 
     * @return The ID of the firewood.
     */
    [[nodiscard]] int get_id() const noexcept override;

    /**
     * @brief Gets the species of the wood.
     * 
     * @return A reference to the species string.
     */
    [[nodiscard]] const std::string& getSpecies() const noexcept;

    /**
     * @brief Gets the drying state of the wood.
     * 
     * @return The drying state.
     */
    [[nodiscard]] Drying getDrying() const noexcept;

    /**
     * @brief Gets the volume of the firewood in cubic feet.
     * 
     * @return The volume in cubic feet.
     */
    [[nodiscard]] unsigned getCubicFeet() const noexcept;

    /**
     * @brief Gets the volume of the firewood in cords.
     * 
     * @return The volume in cords (1 cord = 128 ft³).
     */
    [[nodiscard]] double getChords() const noexcept;

    /**
     * @brief Gets the location where the firewood is stored.
     * 
     * @return A reference to the location string.
     */
    [[nodiscard]] const std::string& getLocation() const noexcept;

    /**
     * @brief Gets additional notes about the firewood.
     * 
     * @return A reference to the notes string.
     */
    [[nodiscard]] const std::string& getNotes() const noexcept;

    /**
     * @brief Gets the length of the log already consumed, in quarters.
     * 
     * @return The consumed length in quarters.
     */
    [[nodiscard]] unsigned getTakenLenQ() const noexcept;

    /**
     * @brief Inserts the firewood object into the database.
     * 
     * @return True if the insertion was successful, false otherwise.
     */
    bool insert() override;

    /**
     * @brief Updates the firewood object in the database.
     * 
     * @return True if the update was successful, false otherwise.
     */
    bool update() override;

    /**
     * @brief Retrieves a Firewood object by its ID from the database.
     * 
     * @param id The ID of the firewood to retrieve.
     * @return An optional Firewood object. If no object is found, the optional is empty.
     */
    static std::optional<Firewood> get_by_id(int id);

    /**
     * @brief Retrieves all Firewood objects from the database.
     * 
     * @return A vector containing all Firewood objects.
     */
    static std::vector<Firewood> get_all();

    /**
     * @brief Manufactures firewood from a log.
     * 
     * @param log The log to manufacture firewood from.
     * @param len_quarters The length of the log to use, in quarters.
     * @param thickness_quarters The thickness of the firewood, in quarters (optional).
     * @param width_quarters The width of the firewood, in quarters (optional).
     * @param drying The drying state of the firewood (optional).
     * @return A vector of Firewood objects created from the log.
     */
    static std::vector<Firewood> make_from_log(
        Log log,
        int len_quarters,
        std::optional<int> thickness_quarters = std::nullopt,
        std::optional<int> width_quarters = std::nullopt,
        std::optional<Drying> drying = std::nullopt);

private:
    /**
     * @brief The unique identifier for the firewood.
     */
    int id_{-1};

    /**
     * @brief The species of the wood.
     */
    std::string species_;

    /**
     * @brief The drying state of the wood.
     */
    Drying drying_{Drying::WET};

    /**
     * @brief The volume of the firewood in cubic feet.
     */
    unsigned cubic_feet_{0};

    /**
     * @brief The length of the log already consumed, in quarters.
     */
    unsigned taken_len_quarters_{0};

    /**
     * @brief The location where the firewood is stored.
     */
    std::string location_;

    /**
     * @brief Additional notes about the firewood.
     */
    std::string notes_;
};

// ---------------------------------------------------------------------------------------------------------------------
//  (Optional) Helper utilities specific to Firewood
//  Add inline helpers here if you later need lightweight calculations.
// ---------------------------------------------------------------------------------------------------------------------
