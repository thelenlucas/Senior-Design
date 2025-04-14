/**
 * @file types.hpp
 * @brief Defines common data types, enumerations, and constants used across the project.
 */
#ifndef TYPES_HPP
#define TYPES_HPP

/**
 * @brief Defines the path to the SQLite database file.
 */
#define DATABASE_FILE "test.db"

/**
 * @brief Represents the drying status of a piece of wood.
 */
enum Drying {
    AIR_DRIED = 0,          ///< Wood has been dried naturally using ambient air flow.
    KILN_DRIED = 1,         ///< Wood has been dried in a controlled kiln environment.
    AIR_AND_KILN_DRIED = 2, ///< Wood has undergone both air drying and kiln drying processes.
    WET = 3,                ///< Wood is freshly cut or has not undergone any significant drying (green).
};

#endif // TYPES_HPP
