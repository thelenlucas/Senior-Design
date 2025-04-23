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

        void addDetails(const std::string &detail)
        {
            detailsLines.push_back(detail);
        }

        // Creates the html article for the product
        inline std::string toHtml(int number) const
        {
            std::string html = "<article class=\"product-card\">\n";
            if (!imageBase64.isEmpty()) {
                html += "<img src=\"data:image/png;base64," + imageBase64.toStdString() + "\" alt=\"Product Image\" class=\"product-image\"/>\n";
            } else {
                html += "<div class=\"placeholder\"></div>\n";
            }
            html += "<div class=\"content\">\n";
            std::ostringstream num_ss;
            num_ss << std::setw(3) << std::setfill('0') << number;
            std::string num_str = num_ss.str();
            html += "<h3>" + toString(type) + " #" + num_str + " - " + species + "</h3>\n";
            if (!detailsLines.empty()) {
                for (const auto &detail : detailsLines) {
                    html += "<p>" + detail + "</p>\n";
                }
            }
            html += "<p class=\"price\">Price: $" + std::to_string(static_cast<int>(std::round(price))) + " / " + pricingUnits + "</p>\n";
            html += "</article>";

            return html;
        }

        // Provides pipe-separated list display for UI
        inline std::string toListString() const
        {
            std::vector<std::string> parts;
            parts.push_back(toString(type) + " - " + species);
            for (const auto &detail : detailsLines) {
                parts.push_back(detail);
            }
            std::ostringstream oss;
            oss << static_cast<int>(std::round(price));
            parts.push_back("$" + oss.str() + " / " + pricingUnits);
            std::string res;
            for (size_t i = 0; i < parts.size(); ++i) {
                res += parts[i];
                if (i + 1 < parts.size()) res += " | ";
            }
            return res;
        }
    };
}

// Makes ProductType and Product usable
Q_DECLARE_METATYPE(woodworks::sales::ProductType)
Q_DECLARE_METATYPE(woodworks::sales::Product)