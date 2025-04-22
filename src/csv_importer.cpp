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

woodworks::domain::types::Drying Importer::returnDryingType(std::string dryStr)
{
    std::transform(dryStr.begin(), dryStr.end(), dryStr.begin(), ::toupper);
    if(dryStr == "KILN DRIED" || dryStr == "KILN") return Drying::KILN_DRIED;
    else if(dryStr == "AIR DRIED" || dryStr == "AIR") return Drying::AIR_DRIED;
    else if(dryStr == "AIR AND KILN DRIED" || dryStr == "KILN AND AIR DRIED" || dryStr == "AIR AND KILN" || dryStr == "KILN AND AIR") return Drying::KILN_AND_AIR_DRIED;
    else return Drying::GREEN;
}

//	   SLAB_SURFACNG: 
//	   woodworks::domain::types::SlabSurfacing
//	   RGH (rough), S1S, S2S

//	   LUMBER_SURFACNG:
//     woodworks::domain::types::LumberSurfacing
//     RGH, S1S, S2S, S3S, S4S

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

/*
void Importer::importSlabs(const std::string& filePath){
    //id, species, thickness (eighths), length (quarters), width (eighths), drying, smoothed, location, notes
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
        uint thickE             = std::stoul(cols[2]);
        uint lenQ               = std::stoul(cols[3]);
        uint widthE             = std::stoul(cols[4]);
        Drying drying           = returnDryingType(cols[5]);
        bool smoothed           = returnSmoothed(cols[6]);
        std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
        std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";

        Slab newSlab(-1, species, thickE, lenQ, widthE, drying, smoothed, location, notes);
        newSlab.insert();
    }
}
*/

/*
void Importer::importCookies(const std::string& filePath){
    //id, from_log, species, thickness (quarters), diameter (quarters), drying, location, notes
    //int, int, string, uint, uint, Drying, string, string
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
        uint diamQ              = std::stoul(cols[3]);
        Drying drying           = returnDryingType(cols[4]);
        std::string location    = (cols.size() > 5 && !cols[5].empty()) ? cols[5] : "";
        std::string notes       = (cols.size() > 6 && !cols[6].empty()) ? cols[6] : "";

        Cookie newCookie(-1, -1, species, thickQ, diamQ, drying, location, notes);
        newCookie.insert();
    }
}
*/

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
