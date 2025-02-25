#include "firewood.hpp"
#include <string>
#include <stdexcept>

Firewood::Firewood(
    int id,
    std::string species,
    Drying drying,
    uint feet_3,
    std::string location,
    std::string notes,
    std::optional<Database *> db
) {
    this->id = id;
    this->species = species;
    this->drying = drying;
    this->feet_3 = feet_3;
    this->location = location;
    this->notes = notes;
}