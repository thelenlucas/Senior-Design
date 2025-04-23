#include "domain/log.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "infra/repository.hpp"
#include "infra/connection.hpp"


namespace woodworks::domain {

    Log::Log(Id id, Species species, Length length,
             Length diameter, Quality quality, Drying drying,
             Dollar cost, std::string location,
             std::string notes, QByteArray imageBuffer)
        : id(id), species(std::move(species)), length(length), diameter(diameter),
        quality(quality), drying(drying), cost(cost),
        location(std::move(location)), notes(std::move(notes)),
        imageBuffer(std::move(imageBuffer))
    {}

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
        Cookie cookie(Id{-1}, species, length, diameter, drying, cookieWorth,
                      "",          // location
                      "",          // notes
                      QByteArray() // imageBuffer
        );

        // Insert the cookie into the database
        auto& deebee = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<Cookie>(deebee);
        repo.add(cookie);
        // Return the cookie
        return cookie;
    }
}