#include <string>
#include <stdexcept>

#ifndef TYPES_HPP
#define TYPES_HPP

// Logs
// Firewood <- Logs
// Live-edge slabs <- Logs
// Cookies <- Logs
// Dimensional lumber <- Logs

/*
Logs table:
serial_number INTEGER PRIMARY KEY AUTOINCREMENT,
location TEXT
length_inches INTEGER,
diameter_inches INTEGER,
price REAL,
species TEXT,
*/
class Log {
public:
    Log(int serial_number, int length, double price, std::string species, std::string surface, std::string location) :
        serial_number_(serial_number), length_(length), price_(price), species_(species), surface_(surface), location_(location) {}

    int getSerialNumber() const { return serial_number_; }
    int getLengthInches() const { return length_; }
    double getPrice() const { return price_; }
    std::string getSpecies() const { return species_; }
    std::string getSurface() const { return surface_; }
    std::string getLocation() const { return location_; }

private:
    int serial_number_;
    int length_;
    double price_;
    std::string species_;
    std::string surface_;
    std::string location_;

    // Cut a length of log from another, reduces the length of the log by the specified amount
    // returns the new log
    Log cutLog(int length) {
        if (length > length_) {
            throw std::runtime_error("Cannot cut a log longer than the original log.");
        }

        length_ -= length;
        return Log(serial_number_, length, price_, species_, surface_, location_);
    }
};

/*
 CREATE TABLE IF NOT EXISTS boards (
serial_number INTEGER PRIMARY KEY AUTOINCREMENT,
in_stock INTEGER,
location TEXT,
species TEXT,
length_inches INTEGER,
thickness_quarters INTEGER,
price_cents_inch INTEGER,
*/
class Board {
private:
    int serial_number_;
    int in_stock_;
    std::string location_;
    std::string species_;
    int length_;
    int width_;
    int thickness_;
    std::string drying_type_;
    int price_cents_inch_;

public:
    Board(int serial_number, int in_stock, std::string location, std::string species, int length, int width, int thickness, std::string drying_type) :
        serial_number_(serial_number), in_stock_(in_stock), location_(location), species_(species), length_(length), width_(width), thickness_(thickness), drying_type_(drying_type) {
        price_cents_inch_ = 0;
    }

    int getSerialNumber() const { return serial_number_; }
    int getInStock() const { return in_stock_; }
    std::string getLocation() const { return location_; }
    std::string getSpecies() const { return species_; }
    int getLengthInches() const { return length_; }
    int getWidthInches() const { return width_; }
    int getThicknessQuarters() const { return thickness_; }
    std::string getDryingType() const { return drying_type_; }
    int getPriceCentsInch() const { return price_cents_inch_; }
};

#endif