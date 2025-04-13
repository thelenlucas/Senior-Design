// =================================================================================================
//  db_views.hpp — SQLite view definitions (header‑only)
//  Generated on 2025‑04‑13
//  Drop this header into your project and call ensureSchema() (see example in previous message) to
//  create / upgrade all views in one shot.
// =================================================================================================
#pragma once

#include <QStringList>

namespace schema {

// Bump whenever you change anything below so ensureSchema() knows to rebuild.
inline constexpr int kSchemaVersion = 1;

inline const QStringList kCreateViews = {
R"sql(
CREATE VIEW IF NOT EXISTS projects AS
SELECT project AS "Name",
       COUNT(*)  AS "Number of Cuts"
FROM   cutlist
GROUP  BY project;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS taken_len_custom AS
SELECT "CUSTOM"   AS type,
       from_log,
       len_quarters
FROM   custom_cuts;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS taken_len_cookies AS
SELECT "COOKIE"           AS type,
       from_log,
       thickness_quarters  AS len_quarters
FROM   cookies;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS taken_len_firewood AS
SELECT "FIREWOOD"          AS type,
       from_log,
       taken_len_quarters   AS len_quarters
FROM   firewood;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS taken_len_partial AS
SELECT type,
       from_log,
       len_quarters
FROM   partial_cuts;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS taken_len_all AS
SELECT * FROM taken_len_cookies
UNION
SELECT * FROM taken_len_custom
UNION
SELECT * FROM taken_len_firewood
UNION
SELECT * FROM taken_len_partial;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_cookies_grouped AS
SELECT COUNT(*)                        AS "Count",
       species                         AS Material,
       ROUND(thickness_quarters / 4.0, 2) AS "Thickness (in)",
       ROUND(diameter_quarters  / 4.0, 2) AS "Diameter (in)"
FROM   cookies
GROUP  BY species,
          ROUND(thickness_quarters / 4.0, 2),
          ROUND(diameter_quarters  / 4.0, 2);
)sql",

/* --------------------------------------------------------------------------------------------------
 *  Unified cutlist views
 * ------------------------------------------------------------------------------------------------*/
R"sql(
CREATE VIEW IF NOT EXISTS display_unified_view AS
SELECT project                                            AS Project,
       part || ' (' || code || ')'                       AS "Part (Code)",
       quantity                                          AS "Qty.",

       /* T dimension formatted in feet‑inch‑fraction */
       CASE
           WHEN t / 8 = 0 THEN
               CASE
                   WHEN t % 8 = 0 THEN '0\'\''
                   WHEN (t % 8) % 2 = 0 THEN CAST((t % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(t % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(t / 8 AS TEXT) ||
               CASE
                   WHEN t % 8 = 0 THEN '\'\''
                   WHEN (t % 8) % 2 = 0 THEN ', ' || CAST((t % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(t % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS T,

       /* W dimension */
       CASE
           WHEN w / 8 = 0 THEN
               CASE
                   WHEN w % 8 = 0 THEN '0\'\''
                   WHEN (w % 8) % 2 = 0 THEN CAST((w % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(w % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(w / 8 AS TEXT) ||
               CASE
                   WHEN w % 8 = 0 THEN '\'\''
                   WHEN (w % 8) % 2 = 0 THEN ', ' || CAST((w % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(w % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS W,

       /* L dimension */
       CASE
           WHEN l / 8 = 0 THEN
               CASE
                   WHEN l % 8 = 0 THEN '0\'\''
                   WHEN (l % 8) % 2 = 0 THEN CAST((l % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(l % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(l / 8 AS TEXT) ||
               CASE
                   WHEN l % 8 = 0 THEN '\'\''
                   WHEN (l % 8) % 2 = 0 THEN ', ' || CAST((l % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(l % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS L,

       (CASE WHEN quantity = progress_rough   THEN '✓ ' ELSE '   ' END) ||
       SUBSTR('###################################################################', 1, progress_rough)  AS "Rhg.",

       (CASE WHEN quantity = progress_finished THEN '✓ ' ELSE '   ' END) ||
       SUBSTR('###################################################################', 1, progress_finished) AS "Finsh.",

       notes                                             AS Notes
FROM   cutlist;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS cutlist_view AS
SELECT project                                            AS Project,
       part || ' (' || code || ')'                       AS "Part (Code)",
       quantity                                          AS "Qty.",
       /* identical dimension logic as above */
       /* T */
       CASE
           WHEN t / 8 = 0 THEN
               CASE
                   WHEN t % 8 = 0 THEN '0\'\''
                   WHEN (t % 8) % 2 = 0 THEN CAST((t % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(t % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(t / 8 AS TEXT) ||
               CASE
                   WHEN t % 8 = 0 THEN '\'\''
                   WHEN (t % 8) % 2 = 0 THEN ', ' || CAST((t % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(t % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS T,
       /* W */
       CASE
           WHEN w / 8 = 0 THEN
               CASE
                   WHEN w % 8 = 0 THEN '0\'\''
                   WHEN (w % 8) % 2 = 0 THEN CAST((w % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(w % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(w / 8 AS TEXT) ||
               CASE
                   WHEN w % 8 = 0 THEN '\'\''
                   WHEN (w % 8) % 2 = 0 THEN ', ' || CAST((w % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(w % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS W,
       /* L */
       CASE
           WHEN l / 8 = 0 THEN
               CASE
                   WHEN l % 8 = 0 THEN '0\'\''
                   WHEN (l % 8) % 2 = 0 THEN CAST((l % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE CAST(l % 8 AS TEXT) || '/8\'\''
               END
           ELSE CAST(l / 8 AS TEXT) ||
               CASE
                   WHEN l % 8 = 0 THEN '\'\''
                   WHEN (l % 8) % 2 = 0 THEN ', ' || CAST((l % 8) / 2 AS TEXT) || '/4\'\''
                   ELSE ', ' || CAST(l % 8 AS TEXT) || '/8\'\''
               END
       END                                              AS L,
       (CASE WHEN quantity = progress_rough   THEN '✓ ' ELSE '   ' END) ||
       SUBSTR('###################################################################', 1, progress_rough)  AS "Rhg.",
       (CASE WHEN quantity = progress_finished THEN '✓ ' ELSE '   ' END) ||
       SUBSTR('###################################################################', 1, progress_finished) AS "Finsh.",
       notes                                             AS Notes
FROM   cutlist;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS logs_view AS
SELECT l.id                                            AS "ID",
       l.species                                       AS "Species",
       CONCAT(FLOOR((l.len_quarters - COALESCE(t.total_taken,0)) / 48), 'ft ',
              FLOOR(((l.len_quarters - COALESCE(t.total_taken,0)) % 48) / 4), 'in') AS "Length",
       CONCAT(FLOOR(l.diameter_quarters / 4), 'in')     AS "Diameter",
       l.quality                                        AS "Quality",
       l.location                                       AS "Location",
       d.string                                         AS "Drying",
       printf('%.2f', ((l.len_quarters - COALESCE(t.total_taken,0)) * l.cost_cents_quarters / 100.0)) AS "Value ($)"
FROM   logs l
LEFT  JOIN (SELECT from_log, SUM(len_quarters) AS total_taken
            FROM   taken_len_all GROUP BY from_log) t ON l.id = t.from_log
LEFT  JOIN drying d ON l.drying = d.int
WHERE  (l.len_quarters - COALESCE(t.total_taken,0)) > 0
  AND  l.scrapped = 0;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_slabs AS
SELECT s.id                                           AS "ID",
       s.species                                      AS "Material",
       ROUND(s.thickness_eights / 8.0, 2)             AS "Thickness (in)",
       ROUND(s.len_quarters   / 48.0, 2)              AS "Length (ft)",
       s.location                                     AS "Location",
       d.string                                       AS "Drying"
FROM   slabs s
LEFT  JOIN drying d ON s.drying = d.int;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_slabs_grouped AS
SELECT COUNT(*)                       AS Count,
       Material,
       "Thickness (in)",
       "Length (ft)",
       Drying,
       Location
FROM   display_slabs
GROUP  BY Material, "Thickness (in)", "Length (ft)", Location, Drying;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_cookies AS
SELECT c.id                                          AS "ID",
       c.species                                     AS "Species",
       ROUND(c.thickness_quarters / 4.0, 2)          AS "Thickness (in)",
       ROUND(c.diameter_quarters  / 4.0, 2)          AS "Diameter (in)",
       c.location                                    AS "Location",
       d.string                                      AS "Drying"
FROM   cookies c
LEFT  JOIN drying d ON c.drying = d.int;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_lumber AS
SELECT l.id                                          AS "ID",
       l.species                                     AS "Material",
       ROUND(l.len_inches / 12.0, 2)                 AS "Length (ft)",
       l.thickness_quarters || '/' || l.width_quarters AS "Size",
       'S' || l.surfacing || 'S'                    AS "Surfacing",
       d.string                                      AS "Drying"
FROM   lumber l
LEFT  JOIN drying d ON l.drying = d.int;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_firewood AS
SELECT f.species                     AS "Species",
       d.string                      AS "Drying",
       ROUND(SUM(f.feet_3) / 128.0, 2) AS "Chords",
       ROUND(SUM(f.feet_3), 2)         AS "Cubic Feet"
FROM   firewood f
LEFT  JOIN drying d ON f.drying = d.int
GROUP  BY f.species, d.string;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS display_lumber_grouped AS
SELECT COUNT(*)                    AS "Count",
       Material,
       "Length (ft)",
       Size,
       Surfacing,
       Drying
FROM   display_lumber
GROUP  BY Material, "Length (ft)", Size, Surfacing, Drying;
)sql",

R"sql(
CREATE VIEW IF NOT EXISTS logs_view_grouped AS
SELECT COUNT(*)                             AS "Count",
       c.species                            AS "Species",
       ROUND(c.thickness_quarters / 4.0, 2) AS "Thickness (in)",
       ROUND(c.diameter_quarters  / 4.0, 2) AS "Diameter (in)",
       c.location                           AS "Location",
       d.string                             AS "Drying"
FROM   cookies c
LEFT  JOIN drying d ON CAST(c.drying AS INTEGER) = d.int
GROUP  BY c.species,
          ROUND(c.thickness_quarters / 4.0, 2),
          ROUND(c.diameter_quarters  / 4.0, 2),
          c.location,
          d.string;
)sql"
};

}