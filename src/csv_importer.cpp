#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "csv_importer.hpp"
#include "domain/log.hpp"

std::vector<std::string> Importer::digestLine(const std::string &line)
{
    std::vector<std::string> parts;
    std::string part;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"')
        {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                part += '"';
                ++i;
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes)
        {
            parts.push_back(part);
            part.clear();
        }
        else
        {
            part += c;
        }
    }
    parts.push_back(part);
    return parts;
}

woodworks::domain::types::Drying Importer::returnDryingType(std::string dryStr)
{
    std::transform(dryStr.begin(), dryStr.end(), dryStr.begin(), ::toupper);
    dryStr.erase(std::remove_if(dryStr.begin(), dryStr.end(),
                                [](char c)
                                { return c == '\r' || c == '\n'; }),
                 dryStr.end());

    bool hasKiln = dryStr.find("KILN") != std::string::npos;
    bool hasAir = dryStr.find("AIR") != std::string::npos;
    bool hasBoth = dryStr.find("BOTH") != std::string::npos;

    if ((hasKiln && hasAir) || hasBoth)
        return Drying::KILN_AND_AIR_DRIED;
    if (hasKiln)
        return Drying::KILN_DRIED;
    if (hasAir)
        return Drying::AIR_DRIED;
    return Drying::GREEN;
}

woodworks::domain::types::SlabSurfacing Importer::returnSurfacingSlabs(std::string surfStr)
{
    std::transform(surfStr.begin(), surfStr.end(), surfStr.begin(), ::toupper);
    surfStr.erase(std::remove_if(surfStr.begin(), surfStr.end(),
                                 [](char c)
                                 { return c == '\r' || c == '\n'; }),
                  surfStr.end());
    if (surfStr == "S1S")
        return SlabSurfacing::S1S;
    else if (surfStr == "S2S")
        return SlabSurfacing::S2S;
    else
        return SlabSurfacing::RGH;
}

woodworks::domain::types::LumberSurfacing Importer::returnSurfacingLumber(std::string surfStr)
{
    std::transform(surfStr.begin(), surfStr.end(), surfStr.begin(), ::toupper);
    surfStr.erase(std::remove_if(surfStr.begin(), surfStr.end(),
                                 [](char c)
                                 { return c == '\r' || c == '\n'; }),
                  surfStr.end());
    if (surfStr == "S1S")
        return LumberSurfacing::S1S;
    else if (surfStr == "S2S")
        return LumberSurfacing::S2S;
    else if (surfStr == "S3S")
        return LumberSurfacing::S3S;
    else if (surfStr == "S4S")
        return LumberSurfacing::S4S;
    else
        return LumberSurfacing::RGH;
}

void Importer::importLogs(const std::string &filePath)
{
    // id, length (ft/in), diameter (in), species, quality, drying, cost, location, notes
    // ignoring, length, length, species, quality, drying, dollar, string, string
    std::ifstream file(filePath);
    if (!file)
    {
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string headerLine, line;
    if (!std::getline(file, headerLine))
    {
        return;
    }
    auto headers = digestLine(headerLine);
    std::unordered_map<std::string, size_t> idx;
    for (size_t i = 0; i < headers.size(); i++)
    {
        idx[headers[i]] = i;
    }

    woodworks::domain::Log log = woodworks::domain::Log::uninitialized();

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        auto cols = digestLine(line);

        auto get = [&](const std::string &name) -> std::string
        {
            auto it = idx.find(name);
            if (it != idx.end())
            {
                size_t i = it->second;
                if (i < cols.size())
                    return cols[i];
                return "";
            }
            // Fallback: scan headers for a case‑insensitive substring match
            std::string nlow = name;
            std::transform(nlow.begin(), nlow.end(), nlow.begin(), ::tolower);
            for (size_t j = 0; j < headers.size(); ++j)
            {
                std::string hlow = headers[j];
                std::transform(hlow.begin(), hlow.end(), hlow.begin(), ::tolower);
                if (hlow.find(nlow) != std::string::npos)
                {
                    if (j < cols.size())
                    {
                        return cols[j];
                    }
                    break;
                }
            }
            return "";
        };

        std::string speciesStr = get("Species");
        std::string lenStr = get("Length");
        std::string diamStr = get("Diameter");
        std::string costStr = get("Cost");
        std::string qualityStr = get("Quality");
        std::string dryingStr = get("Drying");
        std::string location = get("Location");
        std::string notes = get("Notes");

        lenStr.erase(std::remove(lenStr.begin(), lenStr.end(), '\"'), lenStr.end());
        size_t pos = lenStr.find('\'');
        std::string ft = lenStr.substr(0, pos);
        std::string in = lenStr.substr(pos + 1);

        Species logSpecies = {speciesStr};
        Length logLen = Length::fromFeet(std::stod(ft)) + Length::fromInches(std::stod(in));
        Length logDiam = Length::fromInches(std::stoul(diamStr));
        Dollar logCost = {static_cast<int>(std::stod(costStr) * 100)};
        Quality logQuality = {std::stoi(qualityStr)};
        Drying logDrying = returnDryingType(dryingStr);

        log.length = logLen;
        log.diameter = logDiam;
        log.species = logSpecies;
        log.quality = logQuality;
        log.drying = logDrying;
        log.cost = logCost;
        log.location = location;
        log.notes = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Log>(db);
        if (!repo.add(log))
        {
            std::cerr << "Failed to insert log: " + db.lastError().text().toStdString() << std::endl;
        }
    }
}

void Importer::importFirewood(const std::string &filePath)
{
    // id, species, feet^3, drying, cost, location, notes
    // ignoring, species, double, drying, dollar, string, string
    std::ifstream file(filePath);
    if (!file)
    {
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string headerLine, line;
    if (!std::getline(file, headerLine))
    {
        return;
    }
    auto headers = digestLine(headerLine);
    std::unordered_map<std::string, size_t> idx;
    for (size_t i = 0; i < headers.size(); i++)
    {
        idx[headers[i]] = i;
    }

    woodworks::domain::Firewood firewood = woodworks::domain::Firewood::uninitialized();

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        auto cols = digestLine(line);

        auto get = [&](const std::string &name) -> std::string
        {
            auto it = idx.find(name);
            if (it != idx.end())
            {
                size_t i = it->second;
                if (i < cols.size())
                    return cols[i];
                return "";
            }
            // Fallback: scan headers for a case‑insensitive substring match
            std::string nlow = name;
            std::transform(nlow.begin(), nlow.end(), nlow.begin(), ::tolower);
            for (size_t j = 0; j < headers.size(); ++j)
            {
                std::string hlow = headers[j];
                std::transform(hlow.begin(), hlow.end(), hlow.begin(), ::tolower);
                if (hlow.find(nlow) != std::string::npos)
                {
                    if (j < cols.size())
                    {
                        return cols[j];
                    }
                    break;
                }
            }
            return "";
        };

        std::string volumeStr = get("Chords");
        std::string speciesStr = get("Species");
        std::string costStr = get("Cost");
        std::string dryingStr = get("Drying");
        std::string location = get("Location");
        std::string notes = get("Notes");

        Species woodSpecies = {speciesStr};
        Drying woodDrying = returnDryingType(dryingStr);
        double ft3 = std::stod(volumeStr);
        Dollar woodCost = {static_cast<int>(std::stod(costStr) * 100)};

        firewood.species = woodSpecies;
        firewood.cubicFeet = ft3;
        firewood.drying = woodDrying;
        firewood.cost = woodCost;
        firewood.location = location;
        firewood.notes = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Firewood>(db);
        if (!repo.add(firewood))
        {
            std::cerr << "Failed to insert firewood: " + db.lastError().text().toStdString() << std::endl;
        }
    }
}

void Importer::importSlabs(const std::string &filePath)
{
    // id, species, length, width, thickness, drying, surfacing, worth, location, notes
    // ignoring, species, length, length, length, drying, SlabSurfacing, dollar, string, string
    std::ifstream file(filePath);
    if (!file)
    {
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string headerLine, line;
    if (!std::getline(file, headerLine))
    {
        return;
    }
    auto headers = digestLine(headerLine);
    std::unordered_map<std::string, size_t> idx;
    for (size_t i = 0; i < headers.size(); i++)
    {
        idx[headers[i]] = i;
    }

    woodworks::domain::LiveEdgeSlab slab = woodworks::domain::LiveEdgeSlab::uninitialized();

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        auto cols = digestLine(line);

        auto get = [&](const std::string &name) -> std::string
        {
            auto it = idx.find(name);
            if (it != idx.end())
            {
                size_t i = it->second;
                if (i < cols.size())
                    return cols[i];
                return "";
            }
            // Fallback: scan headers for a case‑insensitive substring match
            std::string nlow = name;
            std::transform(nlow.begin(), nlow.end(), nlow.begin(), ::tolower);
            for (size_t j = 0; j < headers.size(); ++j)
            {
                std::string hlow = headers[j];
                std::transform(hlow.begin(), hlow.end(), hlow.begin(), ::tolower);
                if (hlow.find(nlow) != std::string::npos)
                {
                    if (j < cols.size())
                    {
                        return cols[j];
                    }
                    break;
                }
            }
            return "";
        };

        std::string speciesStr = get("Species");
        std::string lenStr = get("Length");
        std::string widthStr = get("Width");
        std::string thickStr = get("Thickness");
        std::string dryingStr = get("Drying");
        std::string surfStr = get("Surfacing");
        std::string costStr = get("Cost");
        std::string location = get("Location");
        std::string notes = get("Notes");

        Species slabSpecies = {speciesStr};
        Length slabLength = Length::fromQuarters(std::stod(lenStr));
        Length slabWidth = Length::fromInches(std::stod(widthStr));
        Length slabThick = Length::fromInches(std::stod(thickStr));
        Drying slabDrying = returnDryingType(dryingStr);
        SlabSurfacing slabSurf = returnSurfacingSlabs(surfStr);
        Dollar slabCost = {static_cast<int>(std::stod(costStr) * 100)};

        slab.species = slabSpecies;
        slab.length = slabLength;
        slab.width = slabWidth;
        slab.thickness = slabThick;
        slab.drying = slabDrying;
        slab.surfacing = slabSurf;
        slab.worth = slabCost;
        slab.location = location;
        slab.notes = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::LiveEdgeSlab>(db);
        if (!repo.add(slab))
        {
            std::cerr << "Failed to insert live edge slab: " + db.lastError().text().toStdString() << std::endl;
        }
    }
}

void Importer::importCookies(const std::string &filePath)
{
    // id, species, length, diameter, drying, worth, location, notes
    // ignoring, species, length, length, drying, dollar, string, string
    std::ifstream file(filePath);
    if (!file)
    {
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string headerLine, line;
    if (!std::getline(file, headerLine))
    {
        return;
    }
    auto headers = digestLine(headerLine);
    std::unordered_map<std::string, size_t> idx;
    for (size_t i = 0; i < headers.size(); i++)
    {
        idx[headers[i]] = i;
    }

    woodworks::domain::Cookie cookie = woodworks::domain::Cookie::uninitialized();

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        auto cols = digestLine(line);

        auto get = [&](const std::string &name) -> std::string
        {
            auto it = idx.find(name);
            if (it != idx.end())
            {
                size_t i = it->second;
                if (i < cols.size())
                    return cols[i];
                return "";
            }
            // Fallback: scan headers for a case‑insensitive substring match
            std::string nlow = name;
            std::transform(nlow.begin(), nlow.end(), nlow.begin(), ::tolower);
            for (size_t j = 0; j < headers.size(); ++j)
            {
                std::string hlow = headers[j];
                std::transform(hlow.begin(), hlow.end(), hlow.begin(), ::tolower);
                if (hlow.find(nlow) != std::string::npos)
                {
                    if (j < cols.size())
                    {
                        return cols[j];
                    }
                    break;
                }
            }
            return "";
        };

        std::string speciesStr = get("Species");
        std::string lengthStr = get("Thickness");
        std::string diamStr = get("Diameter");
        std::string dryingStr = get("Drying");
        std::string costStr = get("Cost");
        std::string location = get("Location");
        std::string notes = get("Notes");

        Species cookieSpecies = {speciesStr};
        Length cookieLen = Length::fromInches(std::stod(lengthStr));
        Length cookieDiam = Length::fromInches(std::stod(diamStr));
        Drying cookieDrying = returnDryingType(dryingStr);
        Dollar cookieCost = {static_cast<int>(std::stod(costStr) * 100)};

        cookie.species = cookieSpecies;
        cookie.length = cookieLen;
        cookie.diameter = cookieDiam;
        cookie.drying = cookieDrying;
        cookie.worth = cookieCost;
        cookie.location = location;
        cookie.notes = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Cookie>(db);
        if (!repo.add(cookie))
        {
            std::cerr << "Failed to insert cookie: " + db.lastError().text().toStdString() << std::endl;
        }
    }
}

void Importer::importLumber(const std::string &filePath)
{
    // id, species, length, width, thickness, drying, surfacing, worth, location, notes
    // ignoring, species, length, length, length, drying, LumberSurfacing, dollar, string, string

    std::ifstream file(filePath);
    if (!file)
    {
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string headerLine, line;
    if (!std::getline(file, headerLine))
    {
        return;
    }
    auto headers = digestLine(headerLine);
    std::unordered_map<std::string, size_t> idx;
    for (size_t i = 0; i < headers.size(); i++)
    {
        idx[headers[i]] = i;
    }

    woodworks::domain::Lumber lumber = woodworks::domain::Lumber::uninitialized();

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        auto cols = digestLine(line);

        auto get = [&](const std::string &name) -> std::string
        {
            auto it = idx.find(name);
            if (it != idx.end())
            {
                size_t i = it->second;
                if (i < cols.size())
                    return cols[i];
                return "";
            }
            // Fallback: scan headers for a case‑insensitive substring match
            std::string nlow = name;
            std::transform(nlow.begin(), nlow.end(), nlow.begin(), ::tolower);
            for (size_t j = 0; j < headers.size(); ++j)
            {
                std::string hlow = headers[j];
                std::transform(hlow.begin(), hlow.end(), hlow.begin(), ::tolower);
                if (hlow.find(nlow) != std::string::npos)
                {
                    if (j < cols.size())
                    {
                        return cols[j];
                    }
                    break;
                }
            }
            return "";
        };

        std::string speciesStr = get("Species");
        std::string lengthStr = get("Length");
        std::string widthStr = get("Width");
        std::string thickStr = get("Thickness");
        std::string surfStr = get("Surfacing");
        std::string dryingStr = get("Drying");
        std::string costStr = get("Cost");
        std::string location = get("Location");
        std::string notes = get("Notes");

        Species lumbSpecies = {speciesStr};
        Length lumbLen = Length::fromQuarters(std::stod(lengthStr));
        Length lumbWid = Length::fromInches(std::stod(widthStr));
        Length lumbThk = Length::fromInches(std::stod(thickStr));
        Drying lumbDry = returnDryingType(dryingStr);
        LumberSurfacing lumbSurf = returnSurfacingLumber(surfStr);
        Dollar lumbCost = {static_cast<int>(std::stod(costStr) * 100)};

        lumber.species = lumbSpecies;
        lumber.length = lumbLen;
        lumber.width = lumbWid;
        lumber.thickness = lumbThk;
        lumber.drying = lumbDry;
        lumber.surfacing = lumbSurf;
        lumber.worth = lumbCost;
        lumber.location = location;
        lumber.notes = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Lumber>(db);
        if (!repo.add(lumber))
        {
            std::cerr << "Failed to insert lumber: " + db.lastError().text().toStdString() << std::endl;
        }
    }
}
