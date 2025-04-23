#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
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
            std::string html = "<article class=\"product_card\">\n";
            if (!imageBase64.isEmpty()) {
                html += "<img src=\"data:image/png;base64," + imageBase64.toStdString() + "\" alt=\"Product Image\" class=\"product_image\"/>\n";
            }
            html += "<div class=\"product_details\">\n";
            html += "<h3>" + toString(type) + " " + std::to_string(number) + " - " + species + "</h3>\n";
            if (!detailsLines.empty()) {
                for (const auto &detail : detailsLines) {
                    html += "<p>" + detail + "</p>\n";
                }
            }
            html += "<p class=\"price\">Price: $" + std::to_string(price) + " / " + pricingUnits + "</p>\n";
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
            oss << std::fixed << std::setprecision(2) << price;
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