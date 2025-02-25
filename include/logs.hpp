#ifndef LOGS_HPP
#define LOGS_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include "interfaces.hpp"

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

    // Scraps a log
    void scrap();

    // Required by Persistent
    int get_id() const override {return id;}
    bool insert() override;
    bool update() override;
    static std::optional<Log> get_by_id(int id); 
    static std::vector<Log> get_all();
};

#endif // TYPES_HPP