#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "csv_importer.hpp"
#include "domain/log.hpp"

std::vector<std::string> Importer::digestLine(const std::string& line){
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while(std::getline(ss, part, ',')){parts.push_back(part);}
    return parts;
}

woodworks::domain::types::Drying Importer::returnDryingType(std::string dryStr){
    std::transform(dryStr.begin(), dryStr.end(), dryStr.begin(), ::toupper);
    if(dryStr == "KILN DRIED" || dryStr == "KILN") return Drying::KILN_DRIED;
    else if(dryStr == "AIR DRIED" || dryStr == "AIR") return Drying::AIR_DRIED;
    else if(dryStr == "AIR AND KILN DRIED" || dryStr == "KILN AND AIR DRIED" || dryStr == "AIR AND KILN" || dryStr == "KILN AND AIR") return Drying::KILN_AND_AIR_DRIED;
    else return Drying::GREEN;
}

//	   SLAB_SURFACNG: 
//	   woodworks::domain::types::SlabSurfacing
//	   RGH (rough), S1S, S2S


woodworks::domain::types::SlabSurfacing Importer::returnSurfacingSlabs(std::string surfStr){
    std::transform(surfStr.begin(), surfStr.end(), surfStr.begin(), ::toupper);
    if(surfStr == "S1S") return SlabSurfacing::S1S;
    else if (surfStr == "S2S") return SlabSurfacing::S2S;
    else return SlabSurfacing::RGH;
}

//	   LUMBER_SURFACNG:
//     woodworks::domain::types::LumberSurfacing
//     RGH, S1S, S2S, S3S, S4S
woodworks::domain::types::LumberSurfacing Importer::returnSurfacingLumber(std::string surfStr){
    std::transform(surfStr.begin(), surfStr.end(), surfStr.begin(), ::toupper);
    if(surfStr == "S1S") return LumberSurfacing::S1S;
    else if (surfStr == "S2S") return LumberSurfacing::S2S;
    else if (surfStr == "S3S") return LumberSurfacing::S3S;
    else if (surfStr == "S4S") return LumberSurfacing::S4S;
    else return LumberSurfacing::RGH;
}

/*
uint Importer::returnSmoothed(std::string smd)
{
    std::transform(smd.begin(), smd.end(), smd.begin(), ::toupper);
    if(smd == "Y" || smd == "YES") return true;
    else return false;
}
*/


void Importer::importLogs(const std::string& filePath){
    //id, length (ft/in), diameter (in), species, quality, drying, cost, location, notes
    //ignoring, length, length, species, quality, drying, dollar, string, string
    std::ifstream file(filePath);
    if(!file){
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string line;
    if(!std::getline(file, line)){return;}

    woodworks::domain::Log log = woodworks::domain::Log::uninitialized();

    while(std::getline(file, line)){
        if(line.empty()){continue;}
        auto cols = digestLine(line);

        cols[2].erase(std::remove(cols[2].begin(), cols[2].end(),'\"'), cols[2].end());
        size_t pos = cols[2].find('\'');
        std::string ft = cols[2].substr(0, pos);
        std::string in = cols[2].substr(pos + 1);

        Species logSpecies          = {cols[1]};
        Length logLen               = Length::fromFeet(std::stod(ft)) + Length::fromInches(std::stod(in));
        Length logDiam              = Length::fromInches(std::stoul(cols[3]));
        Dollar logCost              = {static_cast<int>(std::stod(cols[4])*100)};
        Quality logQuality          = {std::stoi(cols[5])};
        Drying logDrying            = returnDryingType(cols[6]);
        std::string location        = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
        std::string notes           = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";

        log.length                  = logLen;
        log.diameter                = logDiam;
        log.species                 = logSpecies;
        log.quality                 = logQuality;
        log.drying                  = logDrying;
        log.cost                    = logCost;
        log.location                = location;
        log.notes                   = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Log>(db);
        if(!repo.add(log)) {std::cerr << "Failed to insert log: " + db.lastError().text().toStdString() << std::endl;}
     }
}

void Importer::importFirewood(const std::string& filePath){
    //id, species, feet^3, drying, cost, location, notes
    //ignoring, species, double, drying, dollar, string, string
    std::ifstream file(filePath);
    if(!file){
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string line;
    if(!std::getline(file, line)){return;}

    woodworks::domain::Firewood firewood = woodworks::domain::Firewood::uninitialized();

    while(std::getline(file, line)){
        if(line.empty()){continue;}
        auto cols = digestLine(line);

        Species woodSpecies         = {cols[1]};
        Drying woodDrying           = returnDryingType(cols[2]);
        double ft3                  = std::stod(cols[3]);
        Dollar woodCost             = {static_cast<int>(std::stod(cols[4])*100)};
        std::string location        = (cols.size() > 5 && !cols[5].empty()) ? cols[5] : "";
        std::string notes           = (cols.size() > 6 && !cols[6].empty()) ? cols[6] : "";

        firewood.species            = woodSpecies;
        firewood.cubicFeet          = ft3;
        firewood.drying             = woodDrying;
        firewood.cost               = woodCost;
        firewood.location           = location;
        firewood.notes              = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Firewood>(db);
        if(!repo.add(firewood)) {std::cerr << "Failed to insert firewood: " + db.lastError().text().toStdString() << std::endl;}
    }
}


void Importer::importSlabs(const std::string& filePath){
    //id, species, length, width, thickness, drying, surfacing, worth, location, notes
    //ignoring, species, length, length, length, drying, SlabSurfacing, dollar, string, string
    std::ifstream file(filePath);
    if(!file){
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string line;
    if(!std::getline(file, line)){return;}

    woodworks::domain::LiveEdgeSlab slab = woodworks::domain::LiveEdgeSlab::uninitialized();

    while(std::getline(file, line)){
        if(line.empty()){continue;}
        auto cols = digestLine(line);

        Species slabSpecies         = {cols[1]};
        Length slabLength           = Length::fromQuarters(std::stod(cols[2]));
        Length slabWidth            = Length::fromInches(std::stod(cols[3]));
        Length slabThick            = Length::fromInches(std::stod(cols[4]));
        Drying slabDrying           = returnDryingType(cols[5]);
        SlabSurfacing slabSurf      = returnSurfacingSlabs(cols[6]);
        Dollar slabCost             = {static_cast<int>(std::stod(cols[7])*100)};
        std::string location        = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";
        std::string notes           = (cols.size() > 9 && !cols[9].empty()) ? cols[9] : "";

        slab.species                = slabSpecies;
        slab.length                 = slabLength;
        slab.width                  = slabWidth;
        slab.thickness              = slabThick;
        slab.drying                 = slabDrying;
        slab.surfacing              = slabSurf;
        slab.worth                  = slabCost;
        slab.location               = location;
        slab.notes                  = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::LiveEdgeSlab>(db);
        if(!repo.add(slab)) {std::cerr << "Failed to insert live edge slab: " + db.lastError().text().toStdString() << std::endl;}
    }
}


void Importer::importCookies(const std::string& filePath){
    //id, species, length, diameter, drying, worth, location, notes
    //ignoring, species, length, length, drying, dollar, string, string
    std::ifstream file(filePath);
    if(!file){
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string line;
    if(!std::getline(file, line)){return;}

    woodworks::domain::Cookie cookie = woodworks::domain::Cookie::uninitialized();

    while(std::getline(file, line)){
        if(line.empty()){continue;}
        auto cols = digestLine(line);

        Species cookieSpecies           = {cols[1]};
        Length cookieLen                = Length::fromInches(std::stod(cols[2]));
        Length cookieDiam               = Length::fromInches(std::stod(cols[3]));
        Drying cookieDrying             = returnDryingType(cols[4]);
        Dollar cookieCost               = {static_cast<int>(std::stod(cols[5])*100)};
        std::string location            = (cols.size() > 6 && !cols[6].empty()) ? cols[6] : "";
        std::string notes               = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";

        cookie.species                  = cookieSpecies;
        cookie.length                   = cookieLen;
        cookie.diameter                 = cookieDiam;
        cookie.drying                   = cookieDrying;
        cookie.worth                    = cookieCost;
        cookie.location                 = location;
        cookie.notes                    = notes;

        auto &db = woodworks::infra::DbConnection::instance();
        auto repo = woodworks::infra::QtSqlRepository<woodworks::domain::Cookie>(db);
        if(!repo.add(cookie)) {std::cerr << "Failed to insert cookie: " + db.lastError().text().toStdString() << std::endl;}
    }
}

/*
void Importer::importLumber(const std::string& filePath){
    //id, species, thickness (quarters), length (quarters), width (quarters), drying, surfacing (smoothed), location, notes
    //int, string, uint, uint, uint, Drying, bool, string, string

    std::ifstream file(filePath);
    if(!file){
        std::cout << "File could not open at: " << filePath << std::endl;
        return;
    }

    std::string line;
    if(!std::getline(file, line)){return;}

    while(std::getline(file, line)){
        if(line.empty()){continue;}
        auto cols = digestLine(line);

        std::string species     = cols[1];
        uint thickQ             = std::stoul(cols[2]);
        uint lenQ               = std::stoul(cols[3]);
        uint widthQ             = std::stoul(cols[4]);
        Drying drying           = returnDryingType(cols[5]);
        bool smoothed           = returnSmoothed(cols[6]);
        std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
        std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";        
        
        Lumber newLumber(species, thickQ, lenQ, widthQ, drying, smoothed, location, notes);
        newLumber.insert();
    }
}
*/
