#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
#include <optional>

namespace wwhg
{

    /* ───────────────── Base product interface ────────────────────────── */
    class WwhgProduct
    {
    public:
        WwhgProduct() = default;
        explicit WwhgProduct(std::optional<std::string> imagePath)
            : imagePath_(std::move(imagePath)) {}
        virtual ~WwhgProduct() = default;
        virtual std::string sectionId() const = 0;    ///< "cookies", "lumber", …
        virtual std::string sectionTitle() const = 0; ///< "Cookies",  "Lumber", …
        virtual std::string cardHtml() const = 0;     ///< <article class="product-card">…</article>
        virtual int numericId() const = 0;            ///< Used only for default image seed
    protected:
        /* Helper for consistent money formatting */
        static std::string dollars(double d)
        {
            std::ostringstream os;
            os << std::fixed << std::setprecision(2) << d;
            return os.str();
        }

        std::optional<std::string> imagePath_;
    };

    /* ───────────────── Enumerations ──────────────────────────────────── */
    enum class WwhgSurfacing
    {
        RGH,
        S2S,
        S3S
    };
    enum class WwhgFinish
    {
        RGH,
        S1S,
        S2S
    };

    /* ───────────────── Derived product classes ───────────────────────── */
    class WwhgCookie : public WwhgProduct
    {
    public:
        int id{};
        std::string species;
        double thickness_in{};
        double diameter_in{};
        double price_cookie{};

        /* --- WwhgProduct impl ---------------------------------------- */
        std::string sectionId() const override { return "cookies"; }
        std::string sectionTitle() const override { return "Cookies"; }
        int numericId() const override { return id; }
        std::string cardHtml() const override
        {
            const std::string &imgSrc = imagePath_.has_value() ? *imagePath_ : "https://picsum.photos/seed/cookie" + std::to_string(id) + "/1920/1080";
            std::ostringstream os;
            os << R"(        <article class="product-card">)"
                  "\n"
                  "          <img src=\"" << imgSrc << "\" alt=\"" << species << " cookie\">\n"
                                                    "          <div class=\"content\">\n"
                                                    "            <h3>Cookie #"
               << id << " – " << species << "</h3>\n"
                                            "            <p>Thickness: "
               << thickness_in << "&nbsp;in</p>\n"
                                  "            <p>Diameter: "
               << diameter_in << "&nbsp;in</p>\n"
                                 "            <p class=\"price\">$"
               << dollars(price_cookie)
               << "&nbsp;/ Cookie</p>\n"
                  "          </div>\n"
                  "        </article>\n";
            return os.str();
        }

        WwhgCookie(int id_, std::string species_, double thick, double diam, double price, std::optional<std::string> imagePath = std::nullopt)
            : WwhgProduct(imagePath), id{id_}, species{std::move(species_)},
              thickness_in{thick}, diameter_in{diam}, price_cookie{price} {}
    };

    class WwhgBoard : public WwhgProduct
    {
    public:
        int id{};
        std::string species;
        std::string size; ///< "2x4"
        unsigned length_ft{};
        WwhgSurfacing surfacing{};
        double price_ft{};

        std::string sectionId() const override { return "lumber"; }
        std::string sectionTitle() const override { return "Lumber"; }
        int numericId() const override { return id; }
        std::string cardHtml() const override
        {
            const char *surf =
                surfacing == WwhgSurfacing::S2S ? "S2S" : surfacing == WwhgSurfacing::S3S ? "S3S"
                                                                                          : "RGH";

            const std::string &imgSrc = imagePath_.has_value() ? *imagePath_ : "https://picsum.photos/seed/lumber" + std::to_string(id) + "/1920/1080";
            std::ostringstream os;
            os << R"(        <article class="product-card">)"
                  "\n"
                  "          <img src=\"" << imgSrc << "\" alt=\"" << species << " lumber\">\n"
                                                    "          <div class=\"content\">\n"
                                                    "            <h3>Board #"
               << id << " – " << species << "</h3>\n"
                                            "            <p>Size: "
               << size << "</p>\n"
                          "            <p>Available length: "
               << length_ft << "&nbsp;ft</p>\n"
                               "            <p>Surfacing: "
               << surf << "</p>\n"
                          "            <p class=\"price\">$"
               << dollars(price_ft)
               << "&nbsp;/ ft</p>\n"
                  "          </div>\n"
                  "        </article>\n";
            return os.str();
        }

        WwhgBoard(int id_, std::string species_, std::string size_,
                  unsigned len_ft, WwhgSurfacing surf, double price, std::optional<std::string> imagePath = std::nullopt)
            : WwhgProduct(imagePath), id{id_}, species{std::move(species_)}, size{std::move(size_)},
              length_ft{len_ft}, surfacing{surf}, price_ft{price} {}
    };

    class WwhgSlab : public WwhgProduct
    {
    public:
        int id{};
        std::string species;
        double width_in{};
        unsigned length_ft{};
        double thickness_in{};
        WwhgFinish finish{};
        double price_ft{};

        std::string sectionId() const override { return "slabs"; }
        std::string sectionTitle() const override { return "Live‑Edge Slabs"; }
        int numericId() const override { return id; }
        std::string cardHtml() const override
        {
            const char *fin =
                finish == WwhgFinish::S1S ? "S1S" : finish == WwhgFinish::S2S ? "S2S"
                                                                              : "RGH";

            const std::string &imgSrc = imagePath_.has_value() ? *imagePath_ : "https://picsum.photos/seed/slab" + std::to_string(id) + "/1920/1080";
            std::ostringstream os;
            os << R"(        <article class="product-card">)"
                  "\n"
                  "          <img src=\"" << imgSrc << "\" alt=\"" << species << " slab\">\n"
                                                    "          <div class=\"content\">\n"
                                                    "            <h3>Slab #"
               << id << " – " << species << "</h3>\n"
                                            "            <p>Width: "
               << width_in << "&nbsp;in</p>\n"
                              "            <p>Available length: "
               << length_ft << "&nbsp;ft</p>\n"
                               "            <p>Thickness: "
               << thickness_in << "&nbsp;in</p>\n"
                                  "            <p>Finish: "
               << fin << "</p>\n"
                         "            <p class=\"price\">$"
               << dollars(price_ft)
               << "&nbsp;/ ft</p>\n"
                  "          </div>\n"
                  "        </article>\n";
            return os.str();
        }

        WwhgSlab(int id_, std::string species_, double width,
                 unsigned len_ft, double thick, WwhgFinish fin, double price, std::optional<std::string> imagePath = std::nullopt)
            : WwhgProduct(imagePath), id{id_}, species{std::move(species_)}, width_in{width},
              length_ft{len_ft}, thickness_in{thick}, finish{fin}, price_ft{price} {}
    };

    class WwhgFirewoodBundle : public WwhgProduct
    {
    public:
        int id{};
        std::string label;
        double volume_ft3{};
        std::string moisture;
        double price_bundle{};

        std::string sectionId() const override { return "firewood"; }
        std::string sectionTitle() const override { return "Firewood Bundles"; }
        int numericId() const override { return id; }
        std::string cardHtml() const override
        {
            const std::string &imgSrc = imagePath_.has_value() ? *imagePath_ : "https://picsum.photos/seed/firewood" + std::to_string(id) + "/1920/1080";
            std::ostringstream os;
            os << R"(        <article class="product-card">)"
                  "\n"
                  "          <img src=\"" << imgSrc << "\" alt=\"" << label << " firewood\">\n"
                                                  "          <div class=\"content\">\n"
                                                  "            <h3>Bundle #"
               << id << " – " << label << "</h3>\n"
                                          "            <p>Volume: "
               << volume_ft3 << "&nbsp;ft³</p>\n"
                                "            <p>Moisture: "
               << moisture << "</p>\n"
                              "            <p class=\"price\">$"
               << dollars(price_bundle) << "</p>\n"
                                           "          </div>\n"
                                           "        </article>\n";
            return os.str();
        }

        WwhgFirewoodBundle(int id_, std::string label_, double vol,
                           std::string moisture_, double price, std::optional<std::string> imagePath = std::nullopt)
            : WwhgProduct(imagePath), id{id_}, label{std::move(label_)}, volume_ft3{vol},
              moisture{std::move(moisture_)}, price_bundle{price} {}
    };

    using WwhgProductPtr = std::unique_ptr<WwhgProduct>;

} // namespace wwhg
