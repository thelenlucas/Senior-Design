CREATE TABLE sqlite_sequence(name,seq);
CREATE VIEW projects AS SELECT project AS "Name", COUNT(*) AS "Number of Cuts"
FROM cutlist
GROUP BY project
/* projects(Name,"Number of Cuts") */;
CREATE TABLE cookies (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, from_log INTEGER NOT NULL REFERENCES logs (id), species TEXT NOT NULL, thickness_quarters INTEGER CHECK ((thickness_quarters > 0)) NOT NULL, diameter_quarters INTEGER NOT NULL CHECK ((diameter_quarters > 0)), drying INTEGER NOT NULL CHECK ((drying BETWEEN 0 AND 3)), location TEXT REFERENCES storage_bins (name), notes TEXT, media BLOB);
CREATE TABLE storage_bins (name TEXT PRIMARY KEY NOT NULL UNIQUE, notes TEXT);
CREATE TABLE custom_cuts (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, from_log INTEGER REFERENCES logs (id) NOT NULL, len_quarters INTEGER NOT NULL CHECK (len_quarters > 0));
CREATE VIEW taken_len_custom AS SELECT
    "CUSTOM" as type,
    from_log,
    len_quarters
FROM custom_cuts
/* taken_len_custom(type,from_log,len_quarters) */;
CREATE TABLE cutlist (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, project TEXT NOT NULL DEFAULT General, part TEXT, code TEXT, cut INTEGER REFERENCES custom_cuts (id), quantity INTEGER NOT NULL DEFAULT (1) CHECK ((quantity > 0)), t INTEGER NOT NULL CHECK ((t > 0)), w INTEGER NOT NULL CHECK ((w > 0)), l INTEGER NOT NULL CHECK ((l > 0)), required_t_eigths INTEGER, required_w_eights INTEGER, required_l_eights INTEGER NOT NULL, species TEXT, progress_rough INTEGER, progress_finished, notes TEXT);
CREATE VIEW taken_len_cookies AS SELECT
    "COOKIE" as type,
    from_log,
    thickness_quarters as len_quarters
from cookies
/* taken_len_cookies(type,from_log,len_quarters) */;
CREATE TABLE firewood (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, from_log INTEGER REFERENCES logs (id) NOT NULL, species TEXT NOT NULL, drying INTEGER NOT NULL CHECK ((drying BETWEEN 0 AND 3)), feet_3 INTEGER CHECK ((feet_3 >= 0)) NOT NULL, location varchar REFERENCES storage_bins (name), notes TEXT, media BLOB, taken_len_quarters INTEGER NOT NULL);
CREATE VIEW taken_len_firewood AS select
    "FIREWOOD" as type,
    from_log,
    taken_len_quarters
FROM firewood
/* taken_len_firewood(type,from_log,taken_len_quarters) */;
CREATE TABLE partial_cuts (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, from_log INTEGER REFERENCES logs (id) NOT NULL, len_quarters INTEGER NOT NULL CHECK ((len_quarters > 0)), type TEXT NOT NULL, num_products_made INTEGER NOT NULL DEFAULT ((1)));
CREATE VIEW taken_len_partial AS SELECT
    type,
    from_log,
    len_quarters
FROM partial_cuts
/* taken_len_partial(type,from_log,len_quarters) */;
CREATE VIEW taken_len_all AS select * from taken_len_cookies
UNION
select * from taken_len_custom
UNION
select * from taken_len_firewood
UNION
select * from taken_len_partial
/* taken_len_all(type,from_log,len_quarters) */;
CREATE VIEW display_cookies_grouped AS SELECT
  COUNT(*) AS 'Count',
  species AS Material,
  ROUND(thickness_quarters / 4.0, 2) AS 'Thickness (in)',
  ROUND(diameter_quarters / 4.0, 2) AS 'Diameter (in)'
FROM cookies
GROUP BY species,
         ROUND(thickness_quarters / 4.0, 2),
         ROUND(diameter_quarters / 4.0, 2)
/* display_cookies_grouped(Count,Material,"Thickness (in)","Diameter (in)") */;
CREATE VIEW display_unified_view AS SELECT 
    project AS Project,
    part || ' (' || code || ')' AS "Part (Code)",
    quantity AS "Qty.",

    CASE
        WHEN t / 8 = 0 THEN 
            CASE
                WHEN t % 8 = 0 THEN '0'''''
                WHEN (t % 8) % 2 = 0 THEN CAST((t % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(t % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(t / 8 AS TEXT) ||
            CASE
                WHEN t % 8 = 0 THEN ''''''
                WHEN (t % 8) % 2 = 0 THEN ', ' || CAST((t % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(w % 8 AS TEXT) || '/8'''''
            END
    END AS T,

    CASE
        WHEN w / 8 = 0 THEN 
            CASE
                WHEN w % 8 = 0 THEN '0'''''
                WHEN (w % 8) % 2 = 0 THEN CAST((w % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(w % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(w / 8 AS TEXT) ||
            CASE
                WHEN w % 8 = 0 THEN ''''''
                WHEN (w % 8) % 2 = 0 THEN ', ' || CAST((w % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(w % 8 AS TEXT) || '/8'''''
            END
    END AS W,

    CASE
        WHEN l / 8 = 0 THEN 
            CASE
                WHEN l % 8 = 0 THEN '0'''''
                WHEN (l % 8) % 2 = 0 THEN CAST((l % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(l % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(l / 8 AS TEXT) ||
            CASE
                WHEN l % 8 = 0 THEN ''''''
                WHEN (l % 8) % 2 = 0 THEN ', ' || CAST((l % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(l % 8 AS TEXT) || '/8'''''
            END
    END AS L,
    
    (CASE
         WHEN quantity = progress_rough THEN '✓ '
         ELSE '   '
     END) || SUBSTR('###################################################################', 1, progress_rough) AS "Rhg.",
    
    (CASE
         WHEN quantity = progress_finished THEN '✓ '
         ELSE '   '
     END) || SUBSTR('###################################################################', 1, progress_finished) AS "Finsh.",
    
    notes AS Notes
  
FROM cutlist
/* display_unified_view(Project,"Part (Code)","Qty.",T,W,L,"Rhg.","Finsh.",Notes) */;
CREATE VIEW cutlist_view AS SELECT 
    project AS Project,
    part || ' (' || code || ')' AS "Part (Code)",
    quantity AS "Qty.",

    CASE
        WHEN t / 8 = 0 THEN 
            CASE
                WHEN t % 8 = 0 THEN '0'''''
                WHEN (t % 8) % 2 = 0 THEN CAST((t % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(t % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(t / 8 AS TEXT) ||
            CASE
                WHEN t % 8 = 0 THEN ''''''
                WHEN (t % 8) % 2 = 0 THEN ', ' || CAST((t % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(t % 8 AS TEXT) || '/8'''''
            END
    END AS T,

    CASE
        WHEN w / 8 = 0 THEN 
            CASE
                WHEN w % 8 = 0 THEN '0'''''
                WHEN (w % 8) % 2 = 0 THEN CAST((w % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(w % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(w / 8 AS TEXT) ||
            CASE
                WHEN w % 8 = 0 THEN ''''''
                WHEN (w % 8) % 2 = 0 THEN ', ' || CAST((w % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(w % 8 AS TEXT) || '/8'''''
            END
    END AS W,

    CASE
        WHEN l / 8 = 0 THEN 
            CASE
                WHEN l % 8 = 0 THEN '0'''''
                WHEN (l % 8) % 2 = 0 THEN CAST((l % 8) / 2 AS TEXT) || '/4'''''
                ELSE CAST(l % 8 AS TEXT) || '/8'''''
            END
        ELSE 
            CAST(l / 8 AS TEXT) ||
            CASE
                WHEN l % 8 = 0 THEN ''''''
                WHEN (l % 8) % 2 = 0 THEN ', ' || CAST((l % 8) / 2 AS TEXT) || '/4'''''
                ELSE ', ' || CAST(l % 8 AS TEXT) || '/8'''''
            END
    END AS L,
    
    (CASE
         WHEN quantity = progress_rough THEN '✓ '
         ELSE '   '
     END) || SUBSTR('###################################################################', 1, progress_rough) AS "Rhg.",
    
    (CASE
         WHEN quantity = progress_finished THEN '✓ '
         ELSE '   '
     END) || SUBSTR('###################################################################', 1, progress_finished) AS "Finsh.",
    
    notes AS Notes
  
FROM cutlist
/* cutlist_view(Project,"Part (Code)","Qty.",T,W,L,"Rhg.","Finsh.",Notes) */;
CREATE TABLE logs (id integer PRIMARY KEY AUTOINCREMENT, species varchar NOT NULL, len_quarters int NOT NULL CHECK ((len_quarters > 0)), diameter_quarters int NOT NULL CHECK ((diameter_quarters > 0)), cost_cents_quarters int NOT NULL CHECK ((cost_cents_quarters > 0)), quality INTEGER CHECK ((quality BETWEEN 1 AND 5)), location TEXT REFERENCES storage_bins (name), notes TEXT, media BLOB, scrapped INTEGER NOT NULL DEFAULT (0), drying INTEGER NOT NULL DEFAULT (3));
CREATE INDEX MatchingLog ON logs (species, len_quarters, diameter_quarters, quality, location);
CREATE TABLE slabs (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, from_log INTEGER REFERENCES logs (id), species TEXT NOT NULL, thickness_eights INTEGER CHECK ((thickness_eights > 0)) NOT NULL, len_quarters INTEGER NOT NULL CHECK ((len_quarters > 0)), drying INTEGER NOT NULL CHECK ((drying BETWEEN 0 AND 3)), smoothed INTEGER CHECK ((smoothed BETWEEN 0 AND 1)) NOT NULL, location TEXT REFERENCES storage_bins (name), notes TEXT, media BLOB, cut INTEGER REFERENCES partial_cuts (id), width_eights INTEGER NOT NULL DEFAULT (1));
CREATE TABLE drying (int PRIMARY KEY, string TEXT);
CREATE VIEW logs_view AS SELECT 
    l.id AS 'ID', 
    l.species AS 'Species',
    CONCAT(
        FLOOR((l.len_quarters - COALESCE(t.total_taken, 0)) / 48), 'ft ',
        FLOOR(((l.len_quarters - COALESCE(t.total_taken, 0)) % 48) / 4), 'in'
    ) AS 'Length',
    CONCAT(FLOOR(l.diameter_quarters / 4), 'in') AS 'Diameter',
    l.quality AS 'Quality',
    l.location AS 'Location',
    d.string AS 'Drying',
    printf('%.2f', ((l.len_quarters - COALESCE(t.total_taken, 0)) * l.cost_cents_quarters / 100.0)) AS 'Value ($)'
FROM logs l
LEFT JOIN (
    SELECT 
        from_log, 
        SUM(len_quarters) AS total_taken
    FROM taken_len_all
    GROUP BY from_log
) t ON l.id = t.from_log
LEFT JOIN drying d ON l.drying = d.int
WHERE (l.len_quarters - COALESCE(t.total_taken, 0)) > 0 
  AND l.scrapped = 0
/* logs_view(ID,Species,Length,Diameter,Quality,Location,Drying,"Value ($)") */;
CREATE VIEW display_slabs AS SELECT
  s.id AS 'ID',
  s.species AS 'Material',
  ROUND(s.thickness_eights / 8.0, 2) AS 'Thickness (in)',
  ROUND(s.len_quarters / 48.0, 2) AS 'Length (ft)',
  s.location AS 'Location',
  d.string AS 'Drying'
FROM slabs s
LEFT JOIN drying d ON s.drying = d.int
/* display_slabs(ID,Material,"Thickness (in)","Length (ft)",Location,Drying) */;
CREATE VIEW display_slabs_grouped AS SELECT
    COUNT(*) as Count,
    Material,
    'Thickness (in)' as 'Thickness (in)',
    'Length (ft)' as 'Length (ft)',
    Drying as 'Drying',
    Location
FROM display_slabs
GROUP BY Material, 'Thickness (in)', 'Length (ft)', Location, Drying
/* display_slabs_grouped(Count,Material,"Thickness (in)","Length (ft)",Drying,Location) */;
CREATE VIEW display_cookies AS SELECT
  c.id AS 'ID',
  c.species AS 'Species',
  ROUND(c.thickness_quarters / 4.0, 2) AS 'Thickness (in)',
  ROUND(c.diameter_quarters / 4.0, 2) AS 'Diameter (in)',
  c.location AS 'Location',
  d.string AS 'Drying'
FROM cookies c
LEFT JOIN drying d ON c.drying = d.int
/* display_cookies(ID,Species,"Thickness (in)","Diameter (in)",Location,Drying) */;
CREATE VIEW display_lumber AS SELECT
    l.id AS 'ID',
    l.species AS 'Material',
    ROUND(l.len_inches / 12, 2) AS 'Length (ft)',
    l.thickness_quarters || '/' || l.width_quarters AS 'Size',
    'S' || l.surfacing || 'S' AS 'Surfacing',
    d.string AS Drying
FROM lumber l
LEFT JOIN drying d ON l.drying = d.int
/* display_lumber(ID,Material,"Length (ft)",Size,Surfacing,Drying) */;
CREATE TABLE lumber (id INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, from_log INTEGER REFERENCES logs (id) NOT NULL, species TEXT NOT NULL, len_inches INTEGER NOT NULL CHECK ((len_inches > 0)), width_quarters INTEGER NOT NULL CHECK ((width_quarters > 0)), thickness_quarters INTEGER NOT NULL CHECK ((thickness_quarters > 0)), drying INTEGER NOT NULL DEFAULT Wet, surfacing INTEGER NOT NULL CHECK ((drying BETWEEN 0 AND 4)) DEFAULT (0), location TEXT REFERENCES storage_bins (name), notes TEXT, media BLOB, cut INTEGER REFERENCES partial_cuts (id) NOT NULL);
CREATE VIEW display_firewood AS SELECT
    f.species AS 'Species',
    d.string AS 'Drying',
    ROUND(SUM(f.feet_3) / 128.0, 2) AS 'Chords',
    ROUND(SUM(f.feet_3), 2) AS 'Cubic Feet'
FROM firewood f
LEFT JOIN drying d ON f.drying = d.int
GROUP BY f.species, d.string
/* display_firewood(Species,Drying,Chords,"Cubic Feet") */;
CREATE VIEW display_lumber_grouped AS SELECT
    COUNT(*) AS 'Count',
    Material,
    'Length (ft)' AS 'Length (ft)',
    Size,
    Surfacing,
    Drying
FROM display_lumber
GROUP BY Material, 'Length (ft)', Size, Surfacing, Drying
/* display_lumber_grouped(Count,Material,"Length (ft)",Size,Surfacing,Drying) */;
CREATE VIEW logs_view_grouped AS SELECT
    COUNT(*) AS 'Count',
    c.species AS 'Species',
    ROUND(c.thickness_quarters / 4.0, 2) AS 'Thickness (in)',
    ROUND(c.diameter_quarters / 4.0, 2) AS 'Diameter (in)',
    c.location AS 'Location',
    d.string AS 'Drying'
FROM cookies c
LEFT JOIN drying d ON CAST(c.drying AS INTEGER) = d.int
GROUP BY
    c.species,
    ROUND(c.thickness_quarters / 4.0, 2),
    ROUND(c.diameter_quarters / 4.0, 2),
    c.location,
    d.string
/* logs_view_grouped(Count,Species,"Thickness (in)","Diameter (in)",Location,Drying) */;
