#ifndef TYPES_HPP
#define TYPES_HPP

#define DATABASE_FILE "test.db"

/* Types header file
 *  - Primarily for user-defined data types
 * Currently only defines the drying data type
*/

enum Drying {
    AIR_DRIED,
    KILN_DRIED,
    AIR_AND_KILN_DRIED,
    WET,
};

#endif // TYPES_HPP