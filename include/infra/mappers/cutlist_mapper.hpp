#pragma once

#include <string>
#include <vector>

#include "domain/cutlist.hpp"
#include "domain/types.hpp"
#include "domain/units.hpp"

#include "view_helpers.hpp"

#include "infra/connection.hpp"

#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <string>

using namespace woodworks::domain::types;
using namespace woodworks::domain::imperial;
using namespace woodworks::infra;

namespace woodworks::domain
{
    inline QString CustomCut::createDbSQL()
    {
        return u8R"(
            CREATE TABLE IF NOT EXISTS cutlist (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                project TEXT NOT NULL,
                part TEXT NOT NULL,
                code TEXT NOT NULL,
                quantity INTEGER NOT NULL,
                t INTEGER NOT NULL,
                w INTEGER NOT NULL,
                l INTEGER NOT NULL,
                species TEXT NOT NULL,
                progress_rough INTEGER NOT NULL,
                progress_finished INTEGER NOT NULL,
                notes TEXT,
                image BLOB
            )
        )";
    }

    // Individual and group views aren't used and are dummies, but are needed to make the template happy
    inline QString CustomCut::individualViewSQL()
    {
        return u8R"(
            CREATE VIEW IF NOT EXISTS cutlist_individual AS
            SELECT * FROM cutlist
        )";
    }

    inline QString CustomCut::groupedViewSQL()
    {
        return u8R"(
            CREATE VIEW IF NOT EXISTS cutlist_grouped AS
            SELECT * FROM cutlist
        )";
    }

    inline QString CustomCut::insertSQL()
    {
        return u8R"(
            INSERT INTO cutlist (
                project, part, code, quantity, t, w, l, species,
                progress_rough, progress_finished, notes, image
            ) VALUES (?,
                ?, ?, ?, ?, ?, ?, ?,
                ?, ?, ?, ?
            )
        )";
    }

    inline QString CustomCut::updateSQL()
    {
        return u8R"(
            UPDATE cutlist SET
                project = ?,
                part = ?,
                code = ?,
                quantity = ?,
                t = ?,
                w = ?,
                l = ?,
                species = ?,
                progress_rough = ?,
                progress_finished = ?,
                notes = ?,
                image = ?
            WHERE id = ?
        )";
    }

    inline QString CustomCut::selectOneSQL()
    {
        return u8R"(
            SELECT * FROM cutlist WHERE id = ?
        )";
    }

    inline QString CustomCut::selectAllSQL()
    {
        return u8R"(
            SELECT * FROM cutlist
        )";
    }

    inline QString CustomCut::deleteSQL()
    {
        return u8R"(
            DELETE FROM cutlist WHERE id = ?
        )";
    }

    inline QString CustomCut::cutlistViewSQLQuery(const QString &project)
    {
        return QStringList{
            "SELECT ",
            "  id as ID,",
            "  part || ' (' || code || ')'           AS \"Part (Code)\", ",
            "  quantity                              AS \"Qty.\", ",

            // Thickness (t)
            "  CASE "
            "    WHEN t = 0 THEN '0\"' "
            "    WHEN t < 192 THEN "
            "      CAST(t/16 AS TEXT) || "
            "        CASE "
            "          WHEN t % 16 = 0  THEN '\"' "
            "          WHEN (t%16) % 8 = 0 THEN ' ' || CAST((t%16)/8 AS TEXT) || '/2\"' "
            "          WHEN (t%16) % 4 = 0 THEN ' ' || CAST((t%16)/4 AS TEXT) || '/4\"' "
            "          WHEN (t%16) % 2 = 0 THEN ' ' || CAST((t%16)/2 AS TEXT) || '/8\"' "
            "          ELSE                  ' ' || CAST(t%16     AS TEXT) || '/16\"' "
            "        END "
            "    ELSE "
            "      CAST(t/192 AS TEXT) || '''' || "
            "        CASE "
            "          WHEN t % 192 = 0 THEN '' "
            "          ELSE "
            "            ' ' || CAST((t%192)/16 AS TEXT) || "
            "              CASE "
            "                WHEN (t%16)    = 0 THEN '\"' "
            "                WHEN (t%16)%8 = 0 THEN ' ' || CAST((t%16)/8 AS TEXT) || '/2\"' "
            "                WHEN (t%16)%4 = 0 THEN ' ' || CAST((t%16)/4 AS TEXT) || '/4\"' "
            "                WHEN (t%16)%2 = 0 THEN ' ' || CAST((t%16)/2 AS TEXT) || '/8\"' "
            "                ELSE                 ' ' || CAST(t%16    AS TEXT) || '/16\"' "
            "              END "
            "        END "
            "  END AS \"T\", ",

            // Width (w)
            "  CASE "
            "    WHEN w = 0 THEN '0\"' "
            "    WHEN w < 192 THEN "
            "      CAST(w/16 AS TEXT) || "
            "        CASE "
            "          WHEN w % 16 = 0  THEN '\"' "
            "          WHEN (w%16) % 8 = 0 THEN ' ' || CAST((w%16)/8 AS TEXT) || '/2\"' "
            "          WHEN (w%16) % 4 = 0 THEN ' ' || CAST((w%16)/4 AS TEXT) || '/4\"' "
            "          WHEN (w%16) % 2 = 0 THEN ' ' || CAST((w%16)/2 AS TEXT) || '/8\"' "
            "          ELSE                  ' ' || CAST(w%16     AS TEXT) || '/16\"' "
            "        END "
            "    ELSE "
            "      CAST(w/192 AS TEXT) || '''' || "
            "        CASE "
            "          WHEN w % 192 = 0 THEN '' "
            "          ELSE "
            "            ' ' || CAST((w%192)/16 AS TEXT) || "
            "              CASE "
            "                WHEN (w%16)    = 0 THEN '\"' "
            "                WHEN (w%16)%8 = 0 THEN ' ' || CAST((w%16)/8 AS TEXT) || '/2\"' "
            "                WHEN (w%16)%4 = 0 THEN ' ' || CAST((w%16)/4 AS TEXT) || '/4\"' "
            "                WHEN (w%16)%2 = 0 THEN ' ' || CAST((w%16)/2 AS TEXT) || '/8\"' "
            "                ELSE                 ' ' || CAST(w%16    AS TEXT) || '/16\"' "
            "              END "
            "        END "
            "  END AS \"W\", ",

            // Length (l)
            "  CASE "
            "    WHEN l = 0 THEN '0\"' "
            "    WHEN l < 192 THEN "
            "      CAST(l/16 AS TEXT) || "
            "        CASE "
            "          WHEN l % 16 = 0  THEN '\"' "
            "          WHEN (l%16) % 8 = 0 THEN ' ' || CAST((l%16)/8 AS TEXT) || '/2\"' "
            "          WHEN (l%16) % 4 = 0 THEN ' ' || CAST((l%16)/4 AS TEXT) || '/4\"' "
            "          WHEN (l%16) % 2 = 0 THEN ' ' || CAST((l%16)/2 AS TEXT) || '/8\"' "
            "          ELSE                  ' ' || CAST(l%16     AS TEXT) || '/16\"' "
            "        END "
            "    ELSE "
            "      CAST(l/192 AS TEXT) || '''' || "
            "        CASE "
            "          WHEN l % 192 = 0 THEN '' "
            "          ELSE "
            "            ' ' || CAST((l%192)/16 AS TEXT) || "
            "              CASE "
            "                WHEN (l%16)    = 0 THEN '\"' "
            "                WHEN (l%16)%8 = 0 THEN ' ' || CAST((l%16)/8 AS TEXT) || '/2\"' "
            "                WHEN (l%16)%4 = 0 THEN ' ' || CAST((l%16)/4 AS TEXT) || '/4\"' "
            "                WHEN (l%16)%2 = 0 THEN ' ' || CAST((l%16)/2 AS TEXT) || '/8\"' "
            "                ELSE                 ' ' || CAST(l%16    AS TEXT) || '/16\"' "
            "              END "
            "        END "
            "  END AS \"L\", ",

            // Progress bars
            "  (CASE WHEN quantity = progress_rough    THEN '✓ ' ELSE '   ' END)"
            "    || SUBSTR('###################################################################',1,progress_rough)"
            "      AS \"Rhg.\", ",
            "  (CASE WHEN quantity = progress_finished THEN '✓ ' ELSE '   ' END)"
            "    || SUBSTR('###################################################################',1,progress_finished)"
            "      AS \"Finsh.\", ",

            // Notes + FROM + WHERE
            "  notes                                 AS Notes ",
            "FROM cutlist ",
            "WHERE project = '%1';"}
            .join("")
            .arg(project);
    }

    // Binding
    inline void CustomCut::bindForInsert(QSqlQuery &query, const CustomCut &cut)
    {
        query.bindValue(0, QString::fromStdString(cut.project));
        query.bindValue(1, QString::fromStdString(cut.part));
        query.bindValue(2, QString::fromStdString(cut.code));
        query.bindValue(3, cut.quantity);
        query.bindValue(4, cut.t.toTicks());
        query.bindValue(5, cut.w.toTicks());
        query.bindValue(6, cut.l.toTicks());
        query.bindValue(7, QString::fromStdString(cut.species));
        query.bindValue(8, cut.progress_rough);
        query.bindValue(9, cut.progress_finished);
        query.bindValue(10, QString::fromStdString(cut.notes));
        query.bindValue(11, cut.imageBuffer);
    }

    inline void CustomCut::bindForUpdate(QSqlQuery &query, const CustomCut &cut)
    {
        bindForInsert(query, cut);
        query.bindValue(12, cut.id.id);
    }

    inline CustomCut CustomCut::fromRecord(const QSqlRecord &record)
    {
        return CustomCut{
            Id{record.value("id").toInt()},
            record.value("project").toString().toStdString(),
            record.value("part").toString().toStdString(),
            record.value("code").toString().toStdString(),
            record.value("quantity").toInt(),
            Length::fromTicks(record.value("t").toInt()),
            Length::fromTicks(record.value("w").toInt()),
            Length::fromTicks(record.value("l").toInt()),
            record.value("species").toString().toStdString(),
            record.value("progress_rough").toInt(),
            record.value("progress_finished").toInt(),
            record.value("notes").toString().toStdString(),
            record.value("image").toByteArray()};
    }

    // Very simple query on the woodworks.db, select all project from cutlist
    inline std::vector<std::string> CustomCut::allProjects() {
        QSqlQuery query;
        query.prepare("SELECT DISTINCT project FROM cutlist");
        query.exec();

        std::vector<std::string> projects;
        while (query.next()) {
            projects.push_back(query.value(0).toString().toStdString());
        }
        return projects;
    }
}