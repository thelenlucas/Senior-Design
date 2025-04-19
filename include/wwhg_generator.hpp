#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include "wwhg_constants.hpp"
#include "wwhg_datamodel.hpp"

namespace wwhg {

/* ─────────────────────────────────────────────────────────────────── */
class WwhgGenerator {
public:
    explicit WwhgGenerator(std::filesystem::path baseDir)
        : baseDir_(std::move(baseDir)) {}

    /* --- Add any product via unique_ptr (core) ------------------- */
    WwhgGenerator& addProduct(WwhgProductPtr p) {
        products_.emplace_back(std::move(p)); return *this;
    }

    /* --- Convenience “emplace” helpers --------------------------- */
    template <class... Args>
    WwhgGenerator& emplaceCookie(Args&&... args) {
        return addProduct(std::make_unique<WwhgCookie>(std::forward<Args>(args)...));
    }

    template <class... Args>
    WwhgGenerator& emplaceBoard(Args&&... args) {
        return addProduct(std::make_unique<WwhgBoard>(std::forward<Args>(args)...));
    }

    template <class... Args>
    WwhgGenerator& emplaceSlab(Args&&... args) {
        return addProduct(std::make_unique<WwhgSlab>(std::forward<Args>(args)...));
    }

    template <class... Args>
    WwhgGenerator& emplaceFirewood(Args&&... args) {
        return addProduct(std::make_unique<WwhgFirewoodBundle>(std::forward<Args>(args)...));
    }

    /* --- Build / save ------------------------------------------- */
    [[nodiscard]] std::string buildHtml() const
    {
        using namespace wwhg::html;
        std::ostringstream os;
        os << kDocType << '\n' << kHtmlOpen << '\n'
           << kHeadOpen << '\n' << kCharsetMeta << '\n' << kViewportMeta
           << '\n' << kTitle << '\n' << kStyle << '\n' << kHeadClose << '\n'
           << kBodyOpen << '\n' << kHeader << '\n'
           << "<main>\n" << renderAllSections() << "</main>\n"
           << kFooter << '\n' << kBodyClose << '\n' << kHtmlClose << '\n';
        return os.str();
    }

    bool save(std::string_view filename = "index.html") const
    {
        std::error_code ec;
        std::filesystem::create_directories(baseDir_, ec);
        if (ec) return false;
        std::ofstream file(baseDir_ / filename,
                           std::ios::out | std::ios::trunc);
        if (!file) return false;
        file << buildHtml();
        return file.good();
    }

private:
    /* --- Group by section & render -------------------------------- */
    std::string renderAllSections() const
    {
        // Order we want to appear
        static const std::vector<std::string> order =
            {"cookies", "lumber", "slabs", "firewood"};

        // Group pointers by section id
        std::unordered_map<std::string, std::vector<const WwhgProduct*>> groups;
        for (const auto& p : products_)
            groups[p->sectionId()].push_back(p.get());

        std::ostringstream out;
        for (const auto& key : order) {
            auto it = groups.find(key);
            if (it == groups.end()) continue;
            const auto& vec = it->second;
            if (vec.empty()) continue;

            out << "    <section id=\"" << key << "\">\n"
                << "      <h2>" << vec.front()->sectionTitle() << "</h2>\n"
                << "      <div class=\"product-grid\">\n";

            int item_number = 1;
            for (const auto* p : vec) {
                out << p->cardHtml(item_number++);
            }
            out << "      </div>\n    </section>\n";
        }
        return out.str();
    }

    /* --- storage -------------------------------------------------- */
    std::vector<WwhgProductPtr> products_;
    std::filesystem::path baseDir_;
};

} // namespace wwhg
