#include "domain/log.hpp"
#include "domain/cookie.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "infra/repository.hpp"
#include "infra/connection.hpp"

#include <cmath>



namespace woodworks::domain {
    Dollar Log::cut(Length cutLength) {
        if (cutLength > length) {
            throw std::invalid_argument("Cut length is greater than log length");
        }
        
        // Update the worth
        // Calculate the ratio of the cut length to the original length
        Length newLength =  length - cutLength;
        std::cout << "Cut length: " << cutLength.toInches() << " inches" << std::endl;
        std::cout << "Old length: " << length.toInches() << " inches" << std::endl;
        std::cout << "New length: " << newLength.toInches() << " inches" << std::endl;

        double ratio = static_cast<double>(newLength.toTicks()) / static_cast<double>(length.toTicks());
        std::cout << "Ratio: " << ratio << std::endl;
        // Update the cost based on the ratio
        auto oldCost = cost;
        cost = Dollar(static_cast<int>(cost.toCents() * ratio));
        // Update the length
        length = newLength;
        std::cout << "New cost: " << cost.toCents() << " cents" << std::endl;
        std::cout << "Saved Length: " << length.toInches() << " inches" << std::endl;

        // Check if the log shoudl be deleted
        auto& deebee = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<Log>(deebee);
        if (length.toTicks() <= 0) {
            // Log is empty, delete it
            repo.remove(id.id);
            std::cout << "Log is empty, deleting it" << std::endl;
            return Dollar(0);
        }

        // Otherwise, update the log in the database
        repo.update(*this);

        // The worth is the original worth of the log - the new worth of the log
        Dollar worth = Dollar(static_cast<int>(oldCost.toCents() - cost.toCents()));
        return worth;
    };

    // Cuts a cookie
    Cookie Log::cutCookie(Length length) {
        // Check if the length is valid
        if (length > this->length) {
            throw std::invalid_argument("Cut length is greater than log length");
        }

        auto cookieWorth = this->cut(length);

        
        // Create the cookie
        Cookie cookie;
        cookie.id = Id{-1};
        cookie.species = species;
        cookie.length = length;
        cookie.diameter = diameter;
        cookie.drying = drying;
        cookie.worth = cookieWorth;
        cookie.location = "";
        cookie.notes = "";

        // Insert the cookie into the database
        auto& deebee = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<Cookie>(deebee);
        repo.add(cookie);
        // Return the cookie
        return cookie;
    }

    // Cuts firewood
    Firewood Log::cutFirewood(Length cutLength) {
        // Check if the length is valid
        if (length > this->length) {
            throw std::invalid_argument("Cut length is greater than log length");
        }
        auto firewoodWorth = this->cut(cutLength);

        // Create the firewood. We give, in general, 120% of the cut length's volume to the firewood
        // to account for air
        double firewoodVolume = cutLength.toFeet() * pow((this->diameter.toFeet() / 2), 2) * 3.14159 * 1.2;
        auto firewood = Firewood::uninitialized();
        firewood.species = species;
        firewood.cubicFeet = firewoodVolume;
        firewood.drying = drying;
        firewood.cost = firewoodWorth;

        auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
        repo.add(firewood);
        // Update the log in the database
        woodworks::infra::QtSqlRepository<Log>::spawn().update(*this);

        return firewood;
    }
}