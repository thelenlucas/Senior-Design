#pragma once
#include <string_view>

namespace wwhg::html {

/* ── Document framing ──────────────────────────────────────────────── */
inline constexpr std::string_view kDocType    = "<!DOCTYPE html>";
inline constexpr std::string_view kHtmlOpen   = "<html lang=\"en\">";
inline constexpr std::string_view kHtmlClose  = "</html>";
inline constexpr std::string_view kHeadOpen   = "<head>";
inline constexpr std::string_view kHeadClose  = "</head>";
inline constexpr std::string_view kBodyOpen   = "<body>";
inline constexpr std::string_view kBodyClose  = "</body>";

/* ── Meta + title ──────────────────────────────────────────────────── */
inline constexpr std::string_view kCharsetMeta  =
    R"(<meta charset="UTF-8">)";
inline constexpr std::string_view kViewportMeta =
    R"(<meta name="viewport" content="width=device-width, initial-scale=1.0">)";
inline constexpr std::string_view kTitle =
    R"(<title>Woodworks Sales</title>)";

/* ── Full style block (unchanged from your design) ─────────────────── */
inline constexpr std::string_view kStyle = R"(
  <style>
    /* palette & typography … */
    :root{--bg:#f5f1e9;--primary:#4a2e19;--accent:#c98942;
          --card-bg:#fdfbf9;--card-shadow:0 4px 6px rgba(0,0,0,.1);}
    *{box-sizing:border-box;}
    body{margin:0;font-family:system-ui,sans-serif;background:var(--bg);
         color:var(--primary);line-height:1.5;}
    header{background:var(--primary);color:#fff;padding:1.25rem 2rem;text-align:center;}
    header h1{margin:0;font-size:2rem;letter-spacing:.05rem;}
    main{padding:2rem max(2rem,5vw);}section{margin-bottom:3rem;}
    section h2{margin-bottom:1rem;font-size:1.5rem;border-left:.25rem solid var(--accent);
               padding-left:.75rem;}
    .product-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));
                  gap:1.5rem;}
    .product-card{background:var(--card-bg);border-radius:.5rem;overflow:hidden;
                  box-shadow:var(--card-shadow);display:flex;flex-direction:column;}
    .product-card img{width:100%;height:180px;object-fit:cover;}
    .product-card .content{padding:1rem;display:flex;flex-direction:column;flex-grow:1;}
    .product-card h3{margin:0 0 .5rem;font-size:1.125rem;}
    .product-card p{margin:.25rem 0;font-size:.95rem;}
    .price{margin-top:auto;font-weight:600;color:var(--accent);font-size:1.1rem;}
    footer{background:var(--primary);color:#fff;text-align:center;padding:1rem;font-size:.9rem;}
    @media(prefers-color-scheme:dark){
      :root{--bg:#1d1b18;--primary:#d6c6b4;--card-bg:#2b2824;--accent:#d6a266;
            --card-shadow:0 4px 6px rgba(0,0,0,.4);}
    }
  </style>
)";

inline constexpr std::string_view kHeader =
    R"(<header><h1>Woodworks Inventory &amp; Sales</h1></header>)";

inline constexpr std::string_view kFooter =
    R"(<footer>&copy; 2025 Woodworks&nbsp;LLC&nbsp;&mdash;&nbsp;All rights reserved.</footer>)";

} // namespace wwhg::html