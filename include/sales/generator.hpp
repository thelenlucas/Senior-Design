#pragma once

#include <string>
#include <vector>
#include "sales/product.hpp"

namespace woodworks::sales
{

    struct SalesPageGenerator
    {
        std::vector<Product> cookies;
        std::vector<Product> lumber;
        std::vector<Product> slabs;
        std::vector<Product> firewood;

        // Adds a product to the appropriate section based on its type
        void addProduct(const Product &p)
        {
            switch (p.type)
            {
            case COOKIE:
                cookies.push_back(p);
                break;
            case LUMBER:
                lumber.push_back(p);
                break;
            case SLAB:
                slabs.push_back(p);
                break;
            case FIREWOOD:
                firewood.push_back(p);
                break;
            default:
                break;
            }
        }

        // Generates the complete HTML page
        std::string generate() const
        {
            std::string html = head;

            auto buildSection = [&](const std::string &title,
                                    const std::string &id,
                                    const std::vector<Product> &items)
            {
                html += "<section id=\"" + id + "\">\n";
                html += "  <h2>" + title + "</h2>\n";
                html += "  <div class=\"product-grid\">\n";
                int idx = 1;
                for (const auto &p : items)
                {
                    html += p.toHtml(idx++) + "\n";
                }
                html += "  </div>\n";
                html += "</section>\n\n";
            };

            buildSection("Cookies", "cookies", cookies);
            buildSection("Lumber", "lumber", lumber);
            buildSection("Live-Edge Slabs", "slabs", slabs);
            buildSection("Firewood Bundles", "firewood", firewood);

            html += footer;
            return html;
        }

    private:
        static inline const std::string head = R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Woodworks Inventory &amp; Sales</title>
  <style>
    /* --- Color palette: warm, wood-inspired tones --- */
    :root {
      --bg: #f5f1e9;
      --primary: #4a2e19;
      --accent: #c98942;
      --card-bg: #fdfbf9;
      --card-shadow: 0 4px 6px rgba(0,0,0,.1);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: system-ui, sans-serif;
      background: var(--bg);
      color: var(--primary);
      line-height: 1.5;
    }
    header { background: var(--primary); color: #fff; padding: 1.25rem 2rem; text-align: center; }
    header h1 { margin: 0; font-size: 2rem; letter-spacing: .05rem; }
    main { padding: 2rem max(2rem, 5vw); }
    section { margin-bottom: 3rem; }
    section h2 { margin-bottom: 1rem; font-size: 1.5rem; border-left: .25rem solid var(--accent); padding-left: .75rem; }
    .product-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); gap: 1.5rem; }
    .product-card { background: var(--card-bg); border-radius: .5rem; overflow: hidden; box-shadow: var(--card-shadow); display: flex; flex-direction: column; }
    .product-card img { width: 100%; height: 180px; object-fit: cover; }
    .product-card .content { padding: 1rem; display: flex; flex-direction: column; flex-grow: 1; }
    .product-card h3 { margin: 0 0 .5rem 0; font-size: 1.125rem; }
    .product-card p { margin: .25rem 0; font-size: .95rem; }
    .price { margin-top: auto; font-weight: 600; color: var(--accent); font-size: 1.1rem; }
    footer { background: var(--primary); color: #fff; text-align: center; padding: 1rem; font-size: .9rem; }
    @media (prefers-color-scheme: dark) {
      :root { --bg: #1d1b18; --primary: #d6c6b4; --card-bg: #2b2824; --accent: #d6a266; --card-shadow: 0 4px 6px rgba(0,0,0,.4); }
    }
  </style>
</head>
<body>
<header>
  <h1>Woodworks Inventory &amp; Sales</h1>
</header>
<main>
)";

        static inline const std::string footer = R"(
</main>
<footer>
  &copy; 2025 Woodworks LLC &mdash; All rights reserved.
</footer>
</body>
</html>
)";
    };

}