#include "domain/cookie.hpp"
#include "domain/firewood.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"

#include "sales/product.hpp"

using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::sales;

//! CAN"T BE INLINE C++ WILL OPTIMIZE IT OUT
//! WHYYYYYYYYYYYYYYY - Lucas

namespace woodworks::domain
{
    Product Cookie::toProduct()
    {
        Product product;
        product.type = COOKIE;
        product.species = species.name;
        product.addDetails("Thickness: " + length.toInchesString());
        product.addDetails("Diameter: " + diameter.toInchesString());
        product.addDetails("Seasoning: " + toString(drying));
        product.price = worth.toCents() / 100.0;
        product.pricingUnits = "Cookie";

        product.imageBase64 = imageBuffer.toBase64();

        return product;
    }

    Product LiveEdgeSlab::toProduct()
    {
        Product product;
        product.type = SLAB;
        product.species = species.name;
        product.addDetails("Width: " + std::to_string(static_cast<int>(width.toInches())));
        product.addDetails("Thickness: " + std::to_string(static_cast<int>(thickness.toInches())));
        product.addDetails("Surfacing: " + toString(surfacing));
        product.addDetails("Seasoning: " + toString(drying));
        product.price = worth.toCents() / 100.0;
        product.pricingUnits = "Foot";

        product.imageBase64 = imageBuffer.toBase64();

        return product;
    }

    Product Lumber::toProduct()
    {
        Product product;
        product.type = LUMBER;
        product.species = species.name;

        // Thickness should be in quarters of an inch as "Q"/4, if it divides evenly
        // otherwise, it should be in eighths of an inch as "E"/8
        if (std::fmod(thickness.toInches(), 0.25) == 0)
        {
            product.addDetails("Thickness: " + std::to_string(thickness.toQuarters()) + "/4");
        }
        else
        {
            product.addDetails("Thickness: " + std::to_string(thickness.toTicks() / 2) + "/8");
        }

        product.addDetails("Width: " + std::to_string(static_cast<int>(width.toInches())) + " inches");
        product.addDetails("Surfacing: " + toString(surfacing));
        product.addDetails("Seasoning: " + toString(drying));
        product.price = worth.toCents() / 100.0;
        product.pricingUnits = "Foot";

        product.imageBase64 = imageBuffer.toBase64();

        return product;
    }

    Product Firewood::toProduct()
    {
        Product product;
        product.type = FIREWOOD;
        product.species = species.name;
        product.addDetails("Seasoning: " + toString(drying));
        product.price = 0.0;
        product.pricingUnits = "Bundle";

        product.imageBase64 = imageBuffer.toBase64();

        return product;
    }
}