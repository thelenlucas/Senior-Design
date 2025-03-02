#include <string>
#include <stdexcept>
#include <optional>
#include "types.hpp"
#include "interfaces.hpp"
#include "manufacturable.hpp"

#define FIREWOOD_LOGGING true

class Firewood : Manufacturable<Firewood>
{
private:
    int id;
    std::string species;
    Drying drying;
    uint feet_3;
    std::string location;
    std::string notes;
    uint taken_len_quarters;

public:
    Firewood(
        int id,
        std::string species,
        Drying drying,
        uint feet_3,
        uint taken_len_quarters,
        std::string location = "",
        std::string notes = ""
    );

    // Getters
    int getId() { return id; }
    std::string getSpecies() { return species; }
    Drying getDrying() { return drying; }
    uint getFeet3() { return feet_3; }
    std::string getLocation() { return location; }
    std::string getNotes() { return notes; }

    // One chord = 128 cubic feet
    float getChords() { return feet_3 / 128.0; }

    // Required for persistent interface
    int get_id() const override { return id; }
    bool insert() override;
    bool update() override;
    static std::optional<Firewood> get_by_id(int id);
    static std::vector<Firewood> get_all();

    // Required for manufacturable interface
    static std::vector<Firewood> make_from_log(
        Log log,
        int len_quarters,
        std::optional<int> thickness_quarters = 0,
        std::optional<int> width_quarters = 0,
        std::optional<Drying> drying = std::nullopt
    );
};
