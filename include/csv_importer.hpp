#pragma once

#include <vector>
#include "domain/types.hpp"
#include "domain/units.hpp"
#include "domain/log.hpp"
#include "domain/firewood.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/cookie.hpp"
#include "domain/lumber.hpp"
#include "infra/connection.hpp"
#include "infra/repository.hpp"

/**
 * @class Importer
 * @brief Provides functionality to import various wood product types from CSV files.
 *
 * This class offers methods to import logs, firewood, slabs, cookies, and lumber.
 * Each import function parses the provided CSV file and stores the data into the application's repository.
 */
class Importer
{
public:
    /**
     * @brief Imports log data from a CSV file.
     * @param filePath Path to the CSV file containing log data.
     */
    void importLogs(const std::string &filePath);

    /**
     * @brief Imports firewood data from a CSV file.
     * @param filePath Path to the CSV file containing firewood data.
     */
    void importFirewood(const std::string &filePath);

    /**
     * @brief Imports slab data from a CSV file.
     * @param filePath Path to the CSV file containing live edge slab data.
     */
    void importSlabs(const std::string &filePath);

    /**
     * @brief Imports cookie data from a CSV file.
     * @param filePath Path to the CSV file containing wood cookie data.
     */
    void importCookies(const std::string &filePath);

    /**
     * @brief Imports lumber data from a CSV file.
     * @param filePath Path to the CSV file containing lumber data.
     */
    void importLumber(const std::string &filePath);

private:
    /**
     * @brief Parses a single line of CSV data into a vector of strings.
     * @param line A line from the CSV file.
     * @return A vector of strings split by commas.
     */
    static std::vector<std::string> digestLine(const std::string &line);

    /**
     * @brief Converts a drying type string into the corresponding Drying enum.
     * @param dryStr The string representing the drying type (e.g., "Kiln Dried").
     * @return The corresponding Drying enum value.
     */
    woodworks::domain::types::Drying returnDryingType(std::string dryStr);

    /**
     * @brief Converts a surfacing description string into a SlabSurfacing enum.
     * @param surfStr The string describing the surfacing type for slabs.
     * @return The corresponding SlabSurfacing enum value.
     */
    woodworks::domain::types::SlabSurfacing returnSurfacingSlabs(std::string surfStr);

    /**
     * @brief Converts a surfacing description string into a LumberSurfacing enum.
     * @param surfStr The string describing the surfacing type for lumber.
     * @return The corresponding LumberSurfacing enum value.
     */
    woodworks::domain::types::LumberSurfacing returnSurfacingLumber(std::string surfStr);
};
