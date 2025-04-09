#include "live_edge_slabs.hpp"

#include <string>
#include <stdexcept>

Slab::Slab(
    int id,
    std::string species,
    uint thickness_quarters,
    uint len_eights,
    uint width_eighths,
    Drying drying,
    bool smoothed,
    std::string location,
    std::string notes
) {
    this->id = id;
    this->species = species;
    this->thickness_eights = thickness_quarters;
    this->len_quarters = len_eights;
    this->width_eighths = width_eighths;
    this->drying = drying;
    this->smoothed = smoothed;
    this->location = location;
    this->notes = notes;
}