#include "live_edge_slabs.hpp"

#include <string>
#include <stdexcept>

Slab::Slab(int id,
    std::string species,
    uint thickness_quarters,
    uint len_quarters,
    Drying drying,
    bool smoothed,
    std::string location,
    std::string notes,
    std::optional<Database*> db
) {
    this->id = id;
    this->species = species;
    this->thickness_quarters = thickness_quarters;
    this->len_quarters = len_quarters;
    this->drying = drying;
    this->smoothed = smoothed;
    this->location = location;
    this->notes = notes;
}