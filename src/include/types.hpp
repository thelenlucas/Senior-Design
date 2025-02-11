#include <string>
#include <stdexcept>

#ifndef TYPES_HPP
#define TYPES_HPP

// CREATE TABLE logs(
// id integer primary key autoincrement,
// species varchar not null,
// len_in int not null,
// diameter_in int not null,
// cost_cents_log int not null,
// quality varchar);

class Log {
private:
    int id;
    std::string species;
    int len_in;
    int diameter_in;
    int cost_cents_log;
    std::string quality;

public:
    Log(int id, std::string species, int len_in, int diameter_in, int cost_cents_log, std::string quality) {
        this->id = id;
        this->species = species;
        this->len_in = len_in;
        this->diameter_in = diameter_in;
        this->cost_cents_log = cost_cents_log;
        this->quality = quality;
    }

    int getId() {
        return id;
    }

    std::string getSpecies() {
        return species;
    }

    int getLenIn() {
        return len_in;
    }

    int getDiameterIn() {
        return diameter_in;
    }

    int getCostCentsLog() {
        return cost_cents_log;
    }

    std::string getQuality() {
        return quality;
    }
};

#endif // TYPES_HPP