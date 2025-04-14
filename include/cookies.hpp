#pragma once
/**
 * @file cookie.hpp
 * @brief Declarations for Cookie data‑model and helper utilities.
 *
 * Heavyweight logic (database persistence, manufacturing, etc.) lives in *cookie.cpp*.
 * Any lightweight geometry helpers that benefit from inlining may remain in this header.
 */

#include <string>
#include <vector>
#include <optional>

#include "types.hpp"        // Drying, etc.
#include "logs.hpp"         // Log class
#include "interfaces.hpp"   // Persistent<>
#include "manufacturable.hpp"

#define COOKIES_LOGGING true

// ---------------------------------------------------------------------------------------------------------------------
//  Cookie – Persistable + Manufacturable cross‑cut disc from a Log
// ---------------------------------------------------------------------------------------------------------------------

/**
 * @class Cookie
 * @brief Represents a manufacturable cookie with various attributes and operations.
 * 
 * The Cookie class provides an interface for creating, managing, and persisting
 * cookie objects. It includes attributes such as species, thickness, diameter,
 * drying method, and location, and supports operations like insertion, updates,
 * and retrieval from a database.
 */
class Cookie final : public Manufacturable<Cookie>
{
public:
    /**
     * @brief Default constructor for Cookie.
     */
    Cookie() = default;

    /**
     * @brief Constructs a Cookie with specified attributes.
     * 
     * @param id Unique identifier for the cookie.
     * @param from_log ID of the log from which the cookie was created.
     * @param species The species of the cookie.
     * @param thickness_quarters Thickness of the cookie in quarters of an inch.
     * @param diameter_quarters Diameter of the cookie in quarters of an inch.
     * @param drying Drying method used for the cookie.
     * @param location Optional location where the cookie is stored.
     * @param notes Optional notes about the cookie.
     */
    Cookie(int id,
           int from_log,
           std::string species,
           unsigned thickness_quarters,
           unsigned diameter_quarters,
           Drying drying,
           std::string location = {},
           std::string notes = {});

    /**
     * @brief Gets the unique identifier of the cookie.
     * @return The ID of the cookie.
     */
    [[nodiscard]] int get_id() const noexcept override { return id_; }

    /**
     * @brief Gets the ID of the log from which the cookie was created.
     * @return The log ID.
     */
    [[nodiscard]] int getFromLogId() const noexcept {
        return from_log_;
    }

    /**
     * @brief Gets the species of the cookie.
     * @return The species as a string.
     */
    [[nodiscard]] const std::string& getSpecies() const noexcept {
        return species_;
    }

    /**
     * @brief Gets the thickness of the cookie in quarters of an inch.
     * @return The thickness in quarters.
     */
    [[nodiscard]] unsigned getThicknessQuarters() const noexcept {
        return thickness_quarters_;
    }

    /**
     * @brief Gets the diameter of the cookie in quarters of an inch.
     * @return The diameter in quarters.
     */
    [[nodiscard]] unsigned getDiameterQuarters() const noexcept {
        return diameter_quarters_;
    }

    /**
     * @brief Gets the drying method used for the cookie.
     * @return The drying method.
     */
    [[nodiscard]] Drying getDrying() const noexcept {
        return drying_;
    }

    /**
     * @brief Gets the location where the cookie is stored.
     * @return The location as a string.
     */
    [[nodiscard]] const std::string& getLocation() const noexcept {
        return location_;
    }

    /**
     * @brief Gets additional notes about the cookie.
     * @return The notes as a string.
     */
    [[nodiscard]] const std::string& getNotes() const noexcept {
        return notes_;
    }

    /**
     * @brief Inserts the cookie into the database.
     * @return True if the insertion was successful, false otherwise.
     */
    bool insert() override;

    /**
     * @brief Updates the cookie in the database.
     * @return True if the update was successful, false otherwise.
     */
    bool update() override;

    /**
     * @brief Retrieves a cookie by its ID from the database.
     * @param id The ID of the cookie to retrieve.
     * @return An optional containing the cookie if found, or std::nullopt if not.
     */
    static std::optional<Cookie> get_by_id(int id);

    /**
     * @brief Retrieves all cookies from the database.
     * @return A vector containing all cookies.
     */
    static std::vector<Cookie> get_all();

    /**
     * @brief Creates cookies from a log with specified attributes.
     * 
     * @param log The log from which cookies are created.
     * @param thickness_quarters Thickness of the cookies in quarters of an inch.
     * @param diameter_quarters Optional diameter of the cookies in quarters of an inch.
     * @param drying Optional drying method for the cookies.
     * @return A vector containing the created cookies.
     */
    static std::vector<Cookie> make_from_log(
        Log log,
        unsigned thickness_quarters,
        std::optional<int> diameter_quarters = std::nullopt,
        std::optional<Drying> drying = std::nullopt);

private:
    int id_{-1}; /**< Unique identifier for the cookie. */
    int from_log_{-1}; /**< ID of the log from which the cookie was created. */
    std::string species_; /**< The species of the cookie. */
    unsigned thickness_quarters_{0}; /**< Thickness of the cookie in quarters of an inch. */
    unsigned diameter_quarters_{0}; /**< Diameter of the cookie in quarters of an inch. */
    Drying drying_{Drying::WET}; /**< Drying method used for the cookie. */
    std::string location_; /**< Location where the cookie is stored. */
    std::string notes_; /**< Additional notes about the cookie. */
};