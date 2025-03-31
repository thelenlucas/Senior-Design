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
    int from_log;
    std::string species;
    uint thickness_quarters;
    uint diameter_quarters;
    Drying drying;
    std::string location;
    std::string notes;

public:
    Cookie(int id,
        int from_log,
        std::string species,
        uint thickness_quarters,
        uint diameter_quarters,
        Drying drying,
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

    //Persistent (copied from firewood.h)
    int get_id() const override {return id;}
    bool insert() override;
    bool update() override;
    static std::optional<Cookie> get_by_id(int id);
    static std::vector<Cookie> get_all();

    static std::vector<Cookie> make_from_log(
        Log log,
        uint thickness_quarters = 0,
        std::optional<int> diameter_quarters = 0,
        std::optional<Drying> drying = std::nullopt
    );
};
