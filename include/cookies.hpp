/* Vestigial SQL call to create a cookie table */

// CREATE TABLE cookies (
//     id                 INTEGER PRIMARY KEY AUTOINCREMENT
//                                UNIQUE
//                                NOT NULL,
//     species            TEXT    NOT NULL,
//     thickness_quarters INTEGER CHECK ( (thickness_quarters > 0) ) 
//                                NOT NULL,
//     diameter_quarters  INTEGER NOT NULL
//                                CHECK ( (diameter_quarters > 0) ),
//     drying             INTEGER NOT NULL
//                                CHECK ( (drying BETWEEN 0 AND 3) ),
//     location           VARCHAR,
//     notes              TEXT,
//     media              BLOB
// );

/* Cookies Header File:
 *  - Takes drying data type from Types header file
 *   - Air, Kiln, Air&Kiln, Wet
 * Cookie objects are given the following:
 *  - ID (int)
 *  - Species (string)
 *  - Thickness (unsigned int)
 *  - Diameter (unsingned int)
 *  - Drying (types enum)
 *  - Cut Location (string)
 *  - User Notes (string)
 * The Cookie class includes GET functions for the above variables
*/
#include <string>
#include <stdexcept>
#include <optional>
#include "types.hpp"

#define COOKIES_LOGGING true

class Database;

class Cookie {
private:
    int id;
    std::string species;
    uint thickness_quarters;
    uint diameter_quarters;
    Drying drying;
    std::string location;
    std::string notes;
public:
    Cookie(int id,
        std::string species,
        uint thickness_quarters,
        uint diameter_quarters,
        Drying drying,
        std::string location = "",
        std::string notes = "",
        std::optional<Database*> db = std::nullopt
    );

    // Getters
    int getId() {return id;}
    std::string getSpecies() {return species;}
    uint getThicknessQuarters() {return thickness_quarters;}
    uint getDiameterQuarters() {return diameter_quarters;}
    Drying getDrying() {return drying;}
    std::string getLocation() {return location;}
    std::string getNotes() {return notes;}
};