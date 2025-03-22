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

#ifndef COOKIES_HPP
#define COOKIES_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include "interfaces.hpp"
#include "manufacturable.hpp"
#include "types.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include <iomanip>
#include <iostream>

#define COOKIES_LOGGING true

class Cookie : Manufacturable<Cookie> 
{
private:
    int id;
    std::string species;
    Drying drying;
    uint thickness_quarters;
    uint diameter_quarters;
    uint taken_len_quarters;
    std::string location;
    std::string notes;
public:
    Cookie(int id,
        std::string species,
        Drying drying,
        uint thickness_quarters,
        uint diameter_quarters,
        uint taken_len_quarters,
        std::string location = "",
        std::string notes = ""
    );

    // Getters
    // int getId() {return id;}
    std::string getSpecies() const {return species;}
    uint getThicknessQuarters() const {return thickness_quarters;}
    uint getDiameterQuarters() const {return diameter_quarters;}
    Drying getDrying() const {return drying;}
    std::string getLocation() const {return location;}
    std::string getNotes() const {return notes;}

    //Persistent (copied from Log.h)
    int get_id() const override {return id;}
    bool insert() override;
    bool update() override;
    static std::optional<Cookie> get_by_id(int id);
    static std::vector<Cookie> get_all();

    static std::vector<Cookie> make_from_log(
        Log log,
        int len_quarters,
        std::optional<int> thickness_quarters = 0,
        std::optional<int> diameter_quarters = 0,
        std::optional<Drying> drying = std::nullopt
    );
};

#endif // TYPES_HPP
