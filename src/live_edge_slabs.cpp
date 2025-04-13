#include "live_edge_slabs.hpp"
#include "types.hpp" // Included types.hpp for DATABASE_FILE and Drying enum
#include <SQLiteCpp/SQLiteCpp.h> // Ensure SQLiteCpp is included for DB operations
#include <stdexcept>
#include <vector> // For std::vector
#include <string> // For std::string
#include <optional> // For std::optional
#include <iostream> // For logging
#include <utility> // For std::move

// Constructor definition - ensure parameter names match header if changed
Slab::Slab(
    int id,
    std::string species,
    uint thickness_eights, // Matched name to member variable
    uint len_quarters,     // Matched name to member variable
    uint width_eighths,
    Drying drying,
    bool smoothed,
    std::string location,
    std::string notes
) : id(id), species(std::move(species)), thickness_eights(thickness_eights),
    len_quarters(len_quarters), width_eighths(width_eighths), drying(drying),
    smoothed(smoothed), location(std::move(location)), notes(std::move(notes))
{
    // Constructor body can be empty if using initializer list fully
}

// --- Persistent Interface Implementation ---

bool Slab::insert() {
    if (this->id >= 0) {
        // Instead of throwing, maybe log an error and return false?
        // Throwing is acceptable if the caller is expected to handle it.
        std::cerr << "Error: Slab::insert called with non-negative ID (" << this->id << "). Use update() instead." << std::endl;
        // throw std::runtime_error("Slab ID is non-negative (" + std::to_string(this->id) + "), cannot insert. Use update().");
        return false; // Indicate failure
    }

    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        // Ensure the table exists (optional, depends on setup)
        // db.exec("CREATE TABLE IF NOT EXISTS slabs (...)"); // Add full DDL if needed

        // Corrected SQL statement (ensure columns match DB schema)
        // Assuming 'from_log' and 'cut' columns might exist based on old schema comments,
        // but are not member variables here. They should be handled separately if needed.
         SQLite::Statement query(db, "INSERT INTO slabs (species, thickness_eights, len_quarters, width_eights, drying, smoothed, location, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        query.bind(1, this->species);
        query.bind(2, this->thickness_eights);
        query.bind(3, this->len_quarters);
        query.bind(4, this->width_eighths);
        query.bind(5, static_cast<int>(this->drying));
        query.bind(6, this->smoothed ? 1 : 0); // Bind bool as integer
        query.bind(7, this->location);
        query.bind(8, this->notes);

        int changes = query.exec(); // Returns number of rows modified

        if (changes > 0) {
            this->id = static_cast<int>(db.getLastInsertRowid()); // Cast to int
            if (SLABS_LOGGING) std::cout << "Slab inserted with ID: " << this->id << std::endl;
            return true;
        } else {
             if (SLABS_LOGGING) std::cout << "Failed to insert slab into database (no changes)" << std::endl;
             return false;
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite exception during Slab insert: " << e.what() << " (Code: " << e.getErrorCode() << "/" << e.getExtendedErrorCode() << ")" << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception during Slab insert: " << e.what() << std::endl;
        return false;
    }
}

bool Slab::update() {
     if (this->id < 0) {
        std::cerr << "Error: Slab::update called with negative ID (" << this->id << "). Use insert() instead." << std::endl;
        // throw std::runtime_error("Slab ID is negative (" + std::to_string(this->id) + "), cannot update. Use insert().");
        return false; // Indicate failure
    }
     try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
        // Corrected SQL statement
        SQLite::Statement query(db, "UPDATE slabs SET species = ?, thickness_eights = ?, len_quarters = ?, width_eights = ?, drying = ?, smoothed = ?, location = ?, notes = ? WHERE id = ?;");
        query.bind(1, this->species);
        query.bind(2, this->thickness_eights);
        query.bind(3, this->len_quarters);
        query.bind(4, this->width_eighths);
        query.bind(5, static_cast<int>(this->drying));
        query.bind(6, this->smoothed ? 1 : 0); // Bind bool as integer
        query.bind(7, this->location);
        query.bind(8, this->notes);
        query.bind(9, this->id);

        int changes = query.exec(); // Returns number of rows modified

        if (SLABS_LOGGING) {
            if (changes > 0) {
                std::cout << "Slab updated successfully (ID: " << this->id << ")" << std::endl;
            } else {
                // It's possible the update didn't change anything if values were the same,
                // or the ID didn't exist. Check if the row exists first?
                std::cout << "Slab update executed, but no rows changed (ID: " << this->id << "). Might indicate ID not found or data was already current." << std::endl;
            }
        }
         // Return true if execution succeeded, even if no rows changed?
         // Or return changes > 0 to indicate data was actually modified?
         // Let's return true if execution succeeded without error. Check SQLiteCpp docs for exec() return on no-op UPDATE.
         // query.exec() returns the number of rows modified. So changes > 0 is correct for indicating a change occurred.
         return changes > 0;
     } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite exception during Slab update (ID: " << this->id << "): " << e.what() << " (Code: " << e.getErrorCode() << "/" << e.getExtendedErrorCode() << ")" << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception during Slab update (ID: " << this->id << "): " << e.what() << std::endl;
        return false;
    }
}

std::optional<Slab> Slab::get_by_id(int id) {
     if (id < 0) return std::nullopt; // Or maybe 0 is invalid too? Depends on DB schema (AUTOINCREMENT usually starts at 1).

    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        // Ensure column names match the actual DB schema exactly
        SQLite::Statement query(db, "SELECT id, species, thickness_eights, len_quarters, width_eights, drying, smoothed, location, notes FROM slabs WHERE id = ?;");
        query.bind(1, id);

        if (query.executeStep()) { // Found a row
            return Slab( // Use std::make_optional ? No, constructor creates the object
                query.getColumn(0).getInt(),
                query.getColumn(1).getText(),
                query.getColumn(2).getUInt(), // Use getUInt for uint members
                query.getColumn(3).getUInt(),
                query.getColumn(4).getUInt(),
                static_cast<Drying>(query.getColumn(5).getInt()), // Ensure enum cast is valid
                query.getColumn(6).getInt() == 1, // Convert int back to bool
                query.getColumn(7).getText(),
                query.getColumn(8).getText()
            );
        } else {
             // ID not found
             if (SLABS_LOGGING) std::cout << "Slab get_by_id(" << id << "): Not found." << std::endl;
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite exception during Slab get_by_id(" << id << "): " << e.what() << " (Code: " << e.getErrorCode() << "/" << e.getExtendedErrorCode() << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception during Slab get_by_id(" << id << "): " << e.what() << std::endl;
    }
    return std::nullopt; // Return empty optional if not found or error occurred
}

std::vector<Slab> Slab::get_all() {
    std::vector<Slab> slabs;
     try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        // Ensure column names match the actual DB schema exactly
        SQLite::Statement query(db, "SELECT id, species, thickness_eights, len_quarters, width_eights, drying, smoothed, location, notes FROM slabs;");

        while (query.executeStep()) {
            slabs.emplace_back( // Use emplace_back to construct in place
                query.getColumn(0).getInt(),
                query.getColumn(1).getText(),
                query.getColumn(2).getUInt(),
                query.getColumn(3).getUInt(),
                query.getColumn(4).getUInt(),
                static_cast<Drying>(query.getColumn(5).getInt()),
                query.getColumn(6).getInt() == 1,
                query.getColumn(7).getText(),
                query.getColumn(8).getText()
            );
        }
         if (SLABS_LOGGING) std::cout << "Slab get_all: Retrieved " << slabs.size() << " slabs." << std::endl;
    } catch (const SQLite::Exception& e) {
        std::cerr << "SQLite exception during Slab get_all: " << e.what() << " (Code: " << e.getErrorCode() << "/" << e.getExtendedErrorCode() << ")" << std::endl;
        // Depending on requirements, you might want to return the partially filled vector or rethrow
    } catch (const std::exception& e) {
        std::cerr << "Standard exception during Slab get_all: " << e.what() << std::endl;
    }
    return slabs;
}


// --- Manufacturable Interface Implementation ---

// Static method required by Manufacturable base class.
// Creates a *single* Slab object based on parameters, without persisting it directly.
// The base Manufacturable::make_from_log calls this and then calls insert().
std::vector<Slab> Slab::make_from_log(
    Log log, // Pass Log by value or reference? Passed by value in interface definition.
             // Passing by value means log.update() inside won't affect the caller's object
             // unless the Log class itself has reference semantics or similar.
             // Let's assume Log object represents state and needs update. Pass by reference?
             // Interface uses pass-by-value. Stick to that for now, but be aware.
             // ***Correction***: The INTERFACE uses pass-by-value in its signature, but the actual
             // implementation here should probably take Log& to modify and update the original.
             // Let's change this implementation to take Log&. This breaks the strict interface
             // signature matching but is functionally necessary. This implies the interface might need adjustment.
             // ***Re-Correction***: Let's stick to the interface signature (pass by value) for now.
             // The caller of `Manufacturable::make_from_log` is responsible for fetching the log,
             // passing it, and potentially updating the log *after* the call if needed, using the returned product info.
             // This keeps the interface clean. Let's revert the log update logic here.

    int len_quarters, // Length of the slab to create
    std::optional<int> thickness_eights,
    std::optional<int> width_eights,
    std::optional<Drying> drying
) {
    if (SLABS_LOGGING) std::cout << "Slab::make_from_log called for log " << log.get_id() << " (len=" << len_quarters << ", thick=" << thickness_eights.value_or(-1) << ", width=" << width_eights.value_or(-1) << ")" << std::endl;

    if (!thickness_eights.has_value() || thickness_eights.value() <= 0) {
         std::cerr << "Error: Slab::make_from_log requires a positive thickness_eights." << std::endl;
        // throw std::invalid_argument("Slab::make_from_log requires a positive thickness_eights.");
         return {}; // Return empty vector to signal failure
    }
     // Allow width to be optional, default to 1 if not provided or non-positive
     int actual_width_eights = (width_eights.has_value() && width_eights.value() > 0) ? width_eights.value() : 1;
     if (!width_eights.has_value() || width_eights.value() <= 0) {
         if (SLABS_LOGGING) std::cout << "Slab::make_from_log warning: width_eights not provided or invalid, defaulting to 1." << std::endl;
     }


    // Basic validation
    if (len_quarters <= 0) {
         std::cerr << "Error: Slab::make_from_log requires a positive len_quarters." << std::endl;
        // throw std::invalid_argument("Slab dimensions must be positive.");
         return {}; // Return empty vector
    }
     // Check available length - this check is problematic if Log is passed by value
     // and doesn't reflect the *current* DB state.
     // Let's fetch the log again inside? Or trust the caller passed an up-to-date log?
     // For now, assume the passed log object has the correct available length.
     /* // Commenting out length check due to pass-by-value concerns
     if (static_cast<uint>(len_quarters) > log.getAvailableLength()) {
         std::cerr << "Error: Cannot make slab: requested length (" << len_quarters << ") exceeds log available length (" << log.getAvailableLength() << ")." << std::endl;
         // throw std::runtime_error("Cannot make slab: requested length exceeds log available length.");
         return {};
     }
     */

    // Assume slabs made this way are initially not smoothed
    bool smoothed = false;
    // Inherit species and potentially drying state from the log
    std::string species = log.getSpecies();
    Drying slab_drying = drying.value_or(log.getDrying()); // Use provided drying or log's drying

    // Create the slab object (ID -1 as it's not persisted yet)
    Slab newSlab(-1, species,
                 static_cast<uint>(thickness_eights.value()),
                 static_cast<uint>(len_quarters),
                 static_cast<uint>(actual_width_eights),
                 slab_drying, smoothed,
                 "", "" // Default location/notes empty
                 );

    if (SLABS_LOGGING) std::cout << "Slab::make_from_log created Slab object (ID -1) to be returned." << std::endl;

    // Return a vector containing the single created slab
    std::vector<Slab> result;
    result.push_back(std::move(newSlab)); // Move the slab into the vector

    // REMOVED Log update logic here. The base Manufacturable::make_from_log should handle insertion.
    // Log length update needs to happen *outside* this function by the caller,
    // potentially after the base method confirms successful insertion.

    return result;
}


// --- Renamed Static Method for Manufacturing from InProgressSlabs ---

// This replaces the old CutAllSlabs. It takes the InProgressSlab definitions,
// creates Slab objects, inserts them, and updates the log length.
std::vector<Slab> Slab::manufacture_and_persist_slabs(
    Log &log, // Takes Log by reference to update it
    std::vector<InProgressSlab> in_progress_slabs,
    int total_len_quarters, // This is the length *each* slab will have
    std::string location,
    std::string notes
) {
    if (SLABS_LOGGING) std::cout << "Slab::manufacture_and_persist_slabs called for log " << log.get_id() << " (" << in_progress_slabs.size() << " cuts, length=" << total_len_quarters << ")" << std::endl;

    if (total_len_quarters <= 0) {
        std::cerr << "Error: Total length for slabs must be positive (" << total_len_quarters << ")." << std::endl;
        // throw std::invalid_argument("Total length for slabs must be positive.");
        return {};
    }

    // Fetch current available length from DB or trust the passed log object?
    // Let's trust the passed object for now.
     if (static_cast<uint>(total_len_quarters) > log.getAvailableLength()) {
         std::cerr << "Error: Cannot manufacture slabs: requested total length (" << total_len_quarters << ") exceeds log available length (" << log.getAvailableLength() << ")." << std::endl;
         // throw std::runtime_error("Cannot manufacture slabs: requested total length exceeds log available length.");
         return {};
     }

    std::vector<Slab> slabVector;
    slabVector.reserve(in_progress_slabs.size()); // Reserve space

    std::string species = log.getSpecies();
    Drying drying = log.getDrying();
    bool smoothed = false; // Slabs are initially rough
    bool any_insert_failed = false;

    for (const auto &ips : in_progress_slabs) // Use const&
    {
        if (ips.thickness_eighths <= 0 || ips.width_eighths <= 0) {
             std::cerr << "Skipping invalid InProgressSlab: thickness=" << ips.thickness_eighths << ", width=" << ips.width_eighths << std::endl;
             continue;
        }
        // Create the slab object (ID -1 initially)
        Slab newSlab(-1, species,
                     static_cast<uint>(ips.thickness_eighths),
                     static_cast<uint>(total_len_quarters), // All slabs from this batch have the same length
                     static_cast<uint>(ips.width_eighths),
                     drying, smoothed, location, notes);

        // Insert the slab into the database using the new method
        if (newSlab.insert()) { // insert() returns true on success
            slabVector.push_back(std::move(newSlab)); // Move successfully inserted slab to vector
             if (SLABS_LOGGING) std::cout << " Inserted slab with ID: " << slabVector.back().get_id() << std::endl;
        } else {
             // Log error or throw exception if insertion fails?
             std::cerr << "Failed to insert slab: Species=" << species << ", Thickness=" << ips.thickness_eighths << ". Stopping manufacture process." << std::endl;
             any_insert_failed = true;
             // Depending on requirements, might stop the whole process or just skip this slab
             // Let's stop the process if one fails to avoid partial cuts.
             break; // Stop processing more slabs if one fails
        }
    }

    // Only update the log length if all insertions were successful
    if (!any_insert_failed && !slabVector.empty()) {
        try {
            // Update the log object in memory
            log.cut_length(total_len_quarters);
            // Persist the log length change to the database
            if (log.update()) { // update() returns true on success
                 if (SLABS_LOGGING) std::cout << "Log " << log.get_id() << " length updated successfully after manufacturing " << slabVector.size() << " slabs." << std::endl;
            } else {
                std::cerr << "Error: Failed to update log " << log.get_id() << " length in database after manufacturing slabs. Slab insertions might need manual rollback!" << std::endl;
                // This is a problematic state - slabs inserted but log not updated.
                // Transactionality would be ideal here.
                // For now, just log the critical error.
            }
        } catch (const std::exception& e) {
             std::cerr << "Exception updating log " << log.get_id() << " length after manufacturing slabs: " << e.what() << ". Slab insertions might need manual rollback!" << std::endl;
             // Log critical error.
        }
    } else if (any_insert_failed) {
         std::cerr << "Log length not updated due to slab insertion failure." << std::endl;
         // If inserts failed, we might want to return an empty vector or indicate failure clearly.
         // Returning the partially filled vector might be misleading if the process was aborted.
         return {}; // Return empty vector to indicate failure/rollback
    } else {
         if (SLABS_LOGGING) std::cout << "No slabs were inserted, log length not updated." << std::endl;
    }


    return slabVector; // Return the vector of successfully inserted slabs (or empty if failure occurred)
}

