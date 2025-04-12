/**
 * @file logs.hpp
 * @brief Defines the Log class representing a log of wood, including its properties and persistence logic.
 */
#ifndef LOGS_HPP
#define LOGS_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include "interfaces.hpp" // Provides Persistent base class
#include <SQLiteCpp/SQLiteCpp.h> // Database access
#include "types.hpp"      // Provides Drying enum
#include <iomanip>
#include <iostream> // For logging

/**
 * @brief Controls verbose logging specific to Log class operations. Set to false to disable.
 */
#define LOGS_LOGGING true

/**
 * @brief Represents a single log of wood, tracking its properties, cost, and status.
 * @details This class manages log data and interacts with the database for persistence,
 *          inheriting from the Persistent<Log> interface.
 */
class Log : public Persistent<Log> {
private:
    int id;                     ///< Unique identifier for the log.
    std::string species;        ///< Tree species of the log.
    uint len_quarters;          ///< Original length of the log in quarters of an inch.
    uint diameter_quarters;     ///< Diameter of the log in quarters of an inch.
    uint cost_cents_quarters;   ///< Cost of the log per quarter inch of length, in cents.
    uint quality;               ///< Quality rating of the log (interpretation depends on application context).
    std::string location;       ///< Physical location where the log is stored.
    std::string notes;          ///< General notes about the log.
    Drying drying;              ///< Current drying status of the log.

public:
    /**
     * @brief Constructs a new Log object.
     * @param id The unique ID of the log (often 0 for new logs before insertion).
     * @param species The tree species.
     * @param len_quarters The original length in quarters of an inch.
     * @param diameter_quarters The diameter in quarters of an inch.
     * @param cost_cents_quarters The cost per quarter inch length, in cents.
     * @param quality A numerical quality rating.
     * @param drying The initial drying status.
     * @param location (Optional) The storage location. Defaults to empty string.
     * @param notes (Optional) Any relevant notes. Defaults to empty string.
     */
    Log(int id,
        std::string species,
        uint len_quarters,
        uint diameter_quarters,
        uint cost_cents_quarters,
        uint quality,
        Drying drying,
        std::string location = "",
        std::string notes = ""
    );

    // --- Getters ---

    /** @brief Gets the species of the log. @return Log species string. */
    std::string getSpecies() const {return species;}

    /** @brief Gets the original length of the log in quarters of an inch. @return Original length. */
    uint getLenQuarters() const {return len_quarters;} 

    /** @brief Gets the diameter of the log in quarters of an inch. @return Diameter. */
    uint getDiameterQuarters() const {return diameter_quarters;} 

    /** @brief Gets the cost per quarter inch length, in cents. @return Cost per quarter inch. */
    uint getCostCentsQuarters() const {return cost_cents_quarters;} 

    /** @brief Gets the quality rating of the log. @return Quality rating. */
    uint getQuality() const {return quality;} 

    /** @brief Gets the storage location of the log. @return Location string. */
    std::string getLocation() const {return location;} 

    /** @brief Gets the notes associated with the log. @return Notes string. */
    std::string getNotes() const {return notes;}

    /** @brief Gets the current drying status of the log. @return Drying enum value. */
    Drying getDrying() const {return drying;}

    /**
     * @brief Calculates the remaining usable length of the log in quarters of an inch.
     * @details This subtracts the total length of all items already manufactured
     *          (e.g., slabs, cookies) from this log's original length (`len_quarters`).
     *          It queries the `taken_len_all` database view to get the used length.
     * @return The currently available length in quarters of an inch. Returns 0 if the
     *         calculated available length is negative (which shouldn't happen ideally).
     */
    uint getAvailableLength() const {
        // The taken_len_all view is the best way to get the taken length, iterate over
        // it where from_log is our id
        if (LOGS_LOGGING) std::cout << "Getting available length for log " << id << std::endl;
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        SQLite::Statement query(db, "SELECT SUM(len_quarters) FROM taken_len_all WHERE from_log = ?");
        query.bind(1, id);
        uint taken_len = 0;
        if (query.executeStep()) {
             // Use getColumn(0).isNull() check before getInt() if SUM can return NULL (empty set)
            if (!query.getColumn(0).isNull()) {
                 taken_len = query.getColumn(0).getInt();
            }
        }
        // Prevent underflow if taken_len somehow exceeds len_quarters
        return (len_quarters >= taken_len) ? (len_quarters - taken_len) : 0;
    }


    /**
     * @brief Marks the log as scrapped or unusable (implementation specific - likely updates database).
     * @details The exact mechanism (e.g., setting a flag, deleting) depends on the implementation in logs.cpp.
     */
    void scrap();

    /**
     * @brief Reduces the recorded original length of the log by a specified amount.
     * @details This might be used for trimming waste or accounting for sections used for non-standard items.
     *          It modifies the `len_quarters` value, affecting future `getAvailableLength` calculations.
     * @param amt_quarters The amount of length to remove, in quarters of an inch.
     */
    void cut_length(uint amt_quarters); // Renamed param for clarity

    // --- Persistent Interface Implementation ---

    /**
     * @brief Gets the unique ID of this log. (Overrides Persistent::get_id).
     * @return The integer ID.
     */
    int get_id() const override {return id;}

    /**
     * @brief Inserts this Log object as a new record in the database. (Overrides Persistent::insert).
     * @return True if insertion was successful, false otherwise. Assigns the new ID on success.
     */
    bool insert() override;

    /**
     * @brief Updates the database record corresponding to this Log object's ID. (Overrides Persistent::update).
     * @return True if update was successful, false otherwise.
     */
    bool update() override;

    /**
     * @brief Retrieves a Log from the database by its ID. (Implements Persistent::get_by_id requirement).
     * @param id The ID of the log to retrieve.
     * @return An std::optional<Log> containing the log if found, std::nullopt otherwise.
     */
    static std::optional<Log> get_by_id(int id); 

    /**
     * @brief Retrieves all Log records from the database. (Implements Persistent::get_all requirement).
     * @return A std::vector<Log> containing all logs.
     */
    static std::vector<Log> get_all();
};

// Note: The #endif should match the #ifndef guard name.
#endif // LOGS_HPP
