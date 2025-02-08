#include <string>
#include <stdexcept>

#ifndef TYPES_HPP
#define TYPES_HPP

/*
Logs table:
id INTEGER PRIMARY KEY AUTOINCREMENT,
                in_stock INTEGER,
                location TEXT,
                species TEXT,
                length_inches INTEGER,
                width_inches INTEGER,
                thickness_quarters INTEGER,
                drying_type TEXT,
*/
class Log {
private:
    int serial_number_;
    int length_inches_;
    double price_;
    std::string species_;
    std::string surface_;
    std::string location_;

public:
    Log(int serial_number, int length_inches, double price, std::string species, std::string surface, std::string location)
        : serial_number_(serial_number), length_inches_(length_inches), price_(price), species_(species), surface_(surface), location_(location) {}
    int getSerialNumber() const { return serial_number_; }
    int getLengthInches() const { return length_inches_; }
    double getPrice() const { return price_; }
    std::string getSpecies() const { return species_; }
    std::string getSurface() const { return surface_; }
    std::string getLocation() const { return location_; }

    // Cuts one log from another log, modifying the length of the current log,
    // and returning a new log with the cut length.
    // Log cut(int length) {
    //     if (length >= length_inches_) {
    //         throw std::invalid_argument("Cannot cut a log longer than the original.");
    //     }

    //     length_inches_ -= length;
    //     return Log(serial_number_, length, price_, species_, surface_, location_);
    // }
};

/*
Boards:
serial_number INTEGER PRIMARY KEY AUTOINCREMENT,
                in_stock INTEGER,
                location TEXT,
                species TEXT,
                length_inches INTEGER,
                width_inches INTEGER,
                thickness_quarters INTEGER,
                drying_type TEXT,
*/
class Board {
private:
    int serial_number_;
    int in_stock_;
    std::string location_;
    std::string species_;
    int length_inches_;
    int width_inches_;
    int thickness_quarters_;
    std::string drying_type_;

public:
    Board(int serial_number, int in_stock, std::string location, std::string species, int length_inches, int width_inches, int thickness_quarters, std::string drying_type)
        : serial_number_(serial_number), in_stock_(in_stock), location_(location), species_(species), length_inches_(length_inches), width_inches_(width_inches), thickness_quarters_(thickness_quarters), drying_type_(drying_type) {}
    int getSerialNumber() const { return serial_number_; }
    int getInStock() const { return in_stock_; }
    std::string getLocation() const { return location_; }
    std::string getSpecies() const { return species_; }
    int getLengthInches() const { return length_inches_; }
    int getWidthInches() const { return width_inches_; }
    int getThicknessQuarters() const { return thickness_quarters_; }
    std::string getDryingType() const { return drying_type_; }
};

#endif