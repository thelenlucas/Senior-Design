/**
 * @file product.hpp
 * @brief Defines the Product struct and ProductType enum for representing and managing sales products.
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <QByteArray>
#include <QListWidgetItem>

namespace woodworks::sales
{
    /**
     * @enum ProductType
     * @brief Enumerates the types of products available for sale.
     */
    enum ProductType
    {
        COOKIE,  ///< Represents a cookie product.
        SLAB,    ///< Represents a slab product.
        LUMBER,  ///< Represents a lumber product.
        FIREWOOD ///< Represents a firewood product.
    };

    /**
     * @brief Converts a ProductType enum value to its string representation.
     *
     * @param type The ProductType value to convert.
     * @return A string representation of the ProductType.
     */
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

    /**
     * @struct Product
     * @brief Represents a product with attributes such as type, species, details, price, and image.
     */
    struct Product
    {
        ProductType type;                      ///< The type of the product.
        std::string species;                   ///< The species of the product.
        std::vector<std::string> detailsLines; ///< Additional details about the product.
        float price;                           ///< The price of the product.
        std::string pricingUnits;              ///< The units for pricing (e.g., per piece, per pound).
        QByteArray imageBase64;                ///< Base64-encoded image data for the product.

        /**
         * @brief Default constructor initializing a product with default values.
         */
        Product() : type(COOKIE), species(""), price(0.0), pricingUnits("") {}

        /**
         * @brief Adds a detail line to the product.
         *
         * @param detail The detail line to add.
         */
        void addDetails(const std::string &detail)
        {
            detailsLines.push_back(detail);
        }

        /**
         * @brief Converts the product to an HTML representation.
         *
         * @param number The product number to include in the HTML.
         * @return A string containing the HTML representation of the product.
         */
        inline std::string toHtml(int number) const
        {
            std::string html = "<article class=\"product-card\">\n";
            if (!imageBase64.isEmpty())
            {
                html += "<img src=\"data:image/png;base64," + imageBase64.toStdString() + "\" alt=\"Product Image\" class=\"product-image\"/>\n";
            }
            else
            {
                html += "<div class=\"placeholder\"></div>\n";
            }
            html += "<div class=\"content\">\n";
            std::ostringstream num_ss;
            num_ss << std::setw(3) << std::setfill('0') << number;
            std::string num_str = num_ss.str();
            html += "<h3>" + toString(type) + " #" + num_str + " - " + species + "</h3>\n";
            if (!detailsLines.empty())
            {
                for (const auto &detail : detailsLines)
                {
                    html += "<p>" + detail + "</p>\n";
                }
            }
            html += "<p class=\"price\">Price: $" + std::to_string(static_cast<int>(std::round(price))) + " / " + pricingUnits + "</p>\n";
            html += "</article>";

            return html;
        }

        /**
         * @brief Converts the product to a pipe-separated string for UI display.
         *
         * @return A string containing the pipe-separated representation of the product.
         */
        inline std::string toListString() const
        {
            std::vector<std::string> parts;
            parts.push_back(toString(type) + " - " + species);
            for (const auto &detail : detailsLines)
            {
                parts.push_back(detail);
            }
            std::ostringstream oss;
            oss << static_cast<int>(std::round(price));
            parts.push_back("$" + oss.str() + " / " + pricingUnits);
            std::string res;
            for (size_t i = 0; i < parts.size(); ++i)
            {
                res += parts[i];
                if (i + 1 < parts.size())
                    res += " | ";
            }
            return res;
        }
    };
}

// Makes ProductType and Product usable in Qt's meta-object system.
Q_DECLARE_METATYPE(woodworks::sales::ProductType)
Q_DECLARE_METATYPE(woodworks::sales::Product)