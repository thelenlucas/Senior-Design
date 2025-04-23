#pragma once

#include <string>
#include <vector>
#include <QByteArray>

namespace woodworks::sales
{
    enum ProductType
    {
        COOKIE,
        SLAB,
        LUMBER,
        FIREWOOD,
    };

    inline static std::string toString(ProductType type)
    {
        switch (type)
        {
        case COOKIE:
            return "Cookie";
        case SLAB:
            return "Slab";
        case LUMBER:
            return "Lumber";
        case FIREWOOD:
            return "Firewood";
        default:
            return "Unknown";
        }
    }

    struct Product
    {
        ProductType type;
        std::string species;
        std::vector<std::string> detailsLines;
        float price;
        std::string pricingUnits;
        QByteArray imageBase64;

        // Empty constructor
        Product() : type(COOKIE), species(""), price(0.0), pricingUnits("") {}

        // Creates a product from a type with toProduct (teehee, rust)
        template <typename T>
        static Product from(const T &other)
        {
            return other.toProduct();
        }

        void addDetails(const std::string &detail)
        {
            detailsLines.push_back(detail);
        }

        // Creates the html article for the product
        inline std::string toHtml(int number) const
        {
            std::string html = "<article class=\"product_card\">\n";
            html += "<img src=\"data:image/png;base64," + imageBase64.toStdString() + "\" alt=\"Product Image\" class=\"product_image\"/>\n";
            html += "<div class=\"product_details\">\n";
            html += "<h3>" + toString(type) + " " + std::to_string(number) + " - " + species + "</h3>\n";
            for (const auto &line : detailsLines)
            {
                html += "<p>" + line + "</p>\n";
            }
            html += "<p class=\"price\">Price: $" + std::to_string(price) + " / " + pricingUnits + "</p>\n";
            html += "</article>";

            return html;
        }
    };
}