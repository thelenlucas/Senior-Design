#ifndef LOGS_HPP
#define LOGS_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include "interfaces.hpp"
#include <SQLiteCpp/SQLiteCpp.h>
#include "types.hpp"
#include <iomanip>
#include <iostream>

#define LOGS_LOGGING true

class Log : public Persistent<Log> {
private:
    int id;
    std::string species;
    uint len_quarters;
    uint diameter_quarters;
    uint cost_cents_quarters;
    uint quality;
    std::string location;
    std::string notes;

public:
    Log(int id,
        std::string species,
        uint len_quarters,
        uint diameter_quarters,
        uint cost_cents_quarters,
        uint quality,
        std::string location = "",
        std::string notes = ""
    );

    // Getters
    std::string getSpecies() const {return species;} 
    uint getLenQuarters() const {return len_quarters;} 
    uint getDiameterQuarters() const {return diameter_quarters;} 
    uint getCostCentsQuarters() const {return cost_cents_quarters;} 
    uint getQuality() const {return quality;} 
    std::string getLocation() const {return location;} 
    std::string getNotes() const {return notes;}

    // Returns the actual available length of the log
    uint getAvailableLength() const {
        // The taken_len_all view is the best way to get the taken length, iterate over
        // it where from_log is our id
        if (LOGS_LOGGING) std::cout << "Getting available length for log " << id << std::endl;
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READONLY);
        SQLite::Statement query(db, "SELECT len_quarters FROM taken_len_all WHERE from_log = ?");
        query.bind(1, id);
        uint taken_len = 0;
        while (query.executeStep()) {
            taken_len += query.getColumn(0).getInt();
        }

        return len_quarters - taken_len;
    }

    // Scraps a log
    void scrap();
    void cut_length(uint amt);

    // Required by Persistent
    int get_id() const override {return id;}
    bool insert() override;
    bool update() override;
    static std::optional<Log> get_by_id(int id); 
    static std::vector<Log> get_all();
};

#endif // TYPES_HPP