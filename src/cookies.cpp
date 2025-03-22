#include "cookies.hpp"
#include "types.hpp"

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>
#include <iostream>

//Schema:
// CREATE TABLE cookies (
//      id                  INTEGER     PRIMARY KEY     AUTOINCREMENT
//      from_log            INTEGER     NOT NULL        FOREIGN KEY
//      species             TEXT        NOT NULL
//      thickness_quarters  INT         NOT NULL        CHECK CONDITION
//      diameter_quarters   INT         NOT NULL        CHECK CONDITION
//      drying              INT         NOT NULL        CHECK CONDITION
//      location            TEXT                        FOREIGN KEY
//      notes               TEXT
//      media               BLOB
// );

/* Still Need (3/18/25):
- Copy the log id the cookie is from, insert into db
*/

Cookie::Cookie(
    int id,
    std::string species,
    Drying drying,
    uint thickness_quarters,
    uint diameter_quarters,
    uint taken_len_quarters,
    std::string location,
    std::string notes
) {
    this->id = id;
    this->species = species;
    this->drying = drying;
    this->thickness_quarters = thickness_quarters;
    this->diameter_quarters = diameter_quarters;
    this->taken_len_quarters = taken_len_quarters;
    this->location = location;
    this->notes = notes;
}

std::optional<Cookie> Cookie::get_by_id(int id) {
    SQLite::Database db("cookies.db", SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM cookies WHERE id = ?;");
    query.bind(1, id);
    if (query.executeStep()) {
        /*ID, Origin Log, Species, Thickness, Diameter, Drying (int), Location, Notes*/
        return Cookie(
            query.getColumn(0).getInt(), 
            query.getColumn(1).getInt(),
            query.getColumn(2).getText(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getText(),
            query.getColumn(7).getText()
        );
    } 
    return std::nullopt;
}

bool Cookie::insert() {
    SQLite::Database db("cookies.db", SQLite::OPEN_READWRITE);
    SQLite::Statement query(db, "INSERT INTO cookies (species, thickness_quarters, diameter_quarters, drying, location, notes) VALUES (?, ?, ?, ?, ?, ?);");
    query.bind(1, this->species);
    query.bind(3, this->thickness_quarters);
    query.bind(4, this->diameter_quarters);
    query.bind(5, this->drying);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    auto ret = query.exec() > 0;
    if (ret) {this->id = db.getLastInsertRowid();}
    if(COOKIES_LOGGING && !ret) {std::cout << "Failed to insert cookie into database" << std::endl;}
    return ret;
}

bool Cookie::update() {
    SQLite::Database db("cookies.db", SQLite::OPEN_READWRITE);
    SQLite::Statement query(db, "UPDATE cookies SET species = ?, thickness_quarters = ?, diameter_quarters = ?, drying = ?, location = ?, notes = ? WHERE id = ?;");
    query.bind(1, this->species);
    query.bind(3, this->thickness_quarters);
    query.bind(4, this->diameter_quarters);
    query.bind(5, this->drying);
    query.bind(6, this->location);
    query.bind(7, this->notes);
    auto ret = query.exec() > 0;
    if(COOKIES_LOGGING && !ret) {std::cout << "Failed to update cookie in database" << std::endl;}
    return ret;
}

std::vector<Cookie> Cookie::get_all() {
    SQLite::Database db("cookies.db", SQLite::OPEN_READONLY);
    SQLite::Statement query(db, "SELECT * FROM cookies;");
    std::vector<Cookie> cookies;
    while(query.executeStep()){
        /*ID, Origin Log, Species, Thickness, Diameter, Drying (int), Location, Notes*/
        cookies.push_back(Cookie(
            query.getColumn(0).getInt(),
            query.getColumn(1).getInt(),
            query.getColumn(2).getText(),
            query.getColumn(3).getInt(),
            query.getColumn(4).getInt(),
            query.getColumn(5).getInt(),
            query.getColumn(6).getText(),
            query.getColumn(7).getText()
        ));
    } return cookies;
}

std::vector<Cookie> Cookie::make_from_log(
    Log log,
    int len_quarters,
    std::optional<int> thickness_quarters,
    std::optional<int> diameter_quarters,
    std::optional<Drying> drying
) {
    float length_feet = len_quarters / 4.0 / 12.0;
    float diameter_feet = log.getDiameterQuarters() / 4.0 / 12.0;
    std::vector<Cookie> cookie;
    cookie.push_back(Cookie(0, log.getSpecies(), drying.value_or(Drying::KILN_DRIED), length_feet, diameter_feet))
    return cookie;
}