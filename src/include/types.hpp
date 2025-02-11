#include <string>
#include <stdexcept>

#ifndef TYPES_HPP
#define TYPES_HPP

// CREATE TABLE logs(
// id integer primary key autoincrement,
// count int not null,
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
    std::string location;

public:
    Log(int id, std::string species, int len_in, int diameter_in, int cost_cents_log, std::string quality, std::string location) {
        this->id = id;
        this->species = species;
        this->len_in = len_in;
        this->diameter_in = diameter_in;
        this->cost_cents_log = cost_cents_log;
        this->quality = quality;
        this->location = location;
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

    std::string getLocation() {
        return location;
    }

    // Cuts a length off of a log, returning a new log,
    // and reducing the length of the original log. Length
    // must be less than or equal to the length of the log.
    Log cut(int length) {
        if (length > len_in) {
            throw std::invalid_argument("Length must be less than or equal to the length of the log");
        }

        len_in -= length;

        return Log(id, species, length, diameter_in, cost_cents_log, quality, location);
    }
};

enum Drying {
    AIR_DRIED,
    KILN_DRIED,
    AIR_AND_KILN_DRIED,
    WET,
};

int dryToInt(Drying drying) {
    switch (drying) {
        case AIR_DRIED:
            return 0;
        case KILN_DRIED:
            return 1;
        case AIR_AND_KILN_DRIED:
            return 2;
        case WET:
            return 3;
        default:
            throw std::invalid_argument("Invalid drying type");
    }
}

Drying intToDry(int drying) {
    switch (drying) {
        case 0:
            return AIR_DRIED;
        case 1:
            return KILN_DRIED;
        case 2:
            return AIR_AND_KILN_DRIED;
        case 3:
            return WET;
        default:
            throw std::invalid_argument("Invalid drying type");
    }
}

class LiveEdgeSlab {
private:
    int id;                 // ID
    std::string species;    // Species of log this came from
    int len_in;             // The length of this type of board in stock
    int thickness_quarters; // The thickness of this board type
    bool surfaced;          // If this board has been surfaced
    Drying drying;          // The drying method of this board

public:
    LiveEdgeSlab(int id, std::string species, int len_in, int thickness_quarters, bool surfaced, Drying drying) {
        this->id = id;
        this->species = species;
        this->len_in = len_in;
        this->thickness_quarters = thickness_quarters;
        this->surfaced = surfaced;
        this->drying = drying;
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

    int getThicknessQuarters() {
        return thickness_quarters;
    }

    bool getSurfaced() {
        return surfaced;
    }

    Drying getDrying() {
        return drying;
    }
};

class Cookie {
private:
    int id;                 // ID
    std::string species;    // Species of log this came from
    int count;              // Number of cookies of this type
    int diameter_in;        // Diameter of the cookie
    int thickness_in;       // Thickness of the cookie
    Drying drying;          // The drying method of this cookie
};

class Firewood {
private:
    int id;                 // ID
    std::string species;    // Species of log this came from
    int feet_cubed;         // The amount of firewood in feet cubed (1 chord = 128 ft^3)
};

class DimensionalLumber {};

#endif // TYPES_HPP