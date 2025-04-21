#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "csv_importer.hpp"

std::vector<std::string> Importer::digestLine(const std::string& line){
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while(std::getline(ss, part, ',')){parts.push_back(part);}
    return parts;
}

// woodworks::domain::types::Drying Importer::returnDryingType(std::string dryStr)
// {
//     std::transform(dryStr.begin(), dryStr.end(), dryStr.begin(), ::toupper);
//     if(dryStr == "KILN DRIED" || dryStr == "KILN") return 1;
//     else if(dryStr == "AIR DRIED" || dryStr == "AIR") return 2;
//     else if(dryStr == "AIR AND KILN DRIED" || dryStr == "KILN AND AIR DRIED" || dryStr == "AIR AND KILN" || dryStr == "KILN AND AIR") return 3;
//     else return 0;
// }

//	   SLAB_SURFACNG: 
//	   woodworks::domain::types::SlabSurfacing
//	   RGH (rough), S1S, S2S

//	   LUMUBER_SURFACNG:
//     woodworks::domain::types::LumberSurfacing
//     RGH, S1S, S2S, S3S, S4S

// bool Importer::returnSmoothed(std::string smd)
// {
//     std::transform(smd.begin(), smd.end(), smd.begin(), ::toupper);
//     if(smd == "Y" || smd == "YES") return true;
//     else return false;
// }


// void Importer::importLogs(const std::string& filePath){
//     //length (ft/in), diameter (in), species, quality, drying, cost, location, notes
//     //length, length, species, quality, drying, dollar, string, string
//     std::ifstream file(filePath);
//     if(!file){
//         std::cout << "File could not open at: " << filePath << std::endl;
//         return;
//     }

//     std::string line;
//     if(!std::getline(file, line)){return;}

//     while(std::getline(file, line)){
//         if(line.empty()){continue;}
//         auto cols = digestLine(line);
        
//         std::string species     = cols[1];
//         uint lenQ           = std::stoul(cols[2]);
//         uint diamQ          = std::stoul(cols[3]);
//         double cost             = std::stod(cols[4]);
//         int quality             = std::stoi(cols[5]);
//         Drying drying           = returnDryingType(cols[6]);
//         std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
//         std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";

//         Log newLog(-1, species, lenQ, diamQ, cost, quality, drying, location, notes);
//         newLog.insert();
//     }
// }

// void Importer::importFirewood(const std::string& filePath){
//     //id, species, drying, feet^3, location, notes
//     //int, string, Drying, uint, string, string
//     std::ifstream file(filePath);
//     if(!file){
//         std::cout << "File could not open at: " << filePath << std::endl;
//         return;
//     }

//     std::string line;
//     if(!std::getline(file, line)){return;}

//     while(std::getline(file, line)){
//         if(line.empty()){continue;}
//         auto cols = digestLine(line);

//         std::string species     = cols[1];
//         Drying drying           = returnDryingType(cols[2]);
//         uint ft3                = std::stoul(cols[3]);
//         std::string location    = (cols.size() > 4 && !cols[4].empty()) ? cols[4] : "";
//         std::string notes       = (cols.size() > 5 && !cols[5].empty()) ? cols[5] : "";

//         uint taken_lenQ         = 150;

//         Firewood newFirewood(-1, species, drying, ft3, taken_lenQ, location, notes);
//         newFirewood.insert();
//     }
// }

// void Importer::importSlabs(const std::string& filePath){
//     //id, species, thickness (eighths), length (quarters), width (eighths), drying, smoothed, location, notes
//     //int, string, uint, uint, uint, Drying, bool, string, string
//     std::ifstream file(filePath);
//     if(!file){
//         std::cout << "File could not open at: " << filePath << std::endl;
//         return;
//     }

//     std::string line;
//     if(!std::getline(file, line)){return;}

//     while(std::getline(file, line)){
//         if(line.empty()){continue;}
//         auto cols = digestLine(line);

//         std::string species     = cols[1];
//         uint thickE             = std::stoul(cols[2]);
//         uint lenQ               = std::stoul(cols[3]);
//         uint widthE             = std::stoul(cols[4]);
//         Drying drying           = returnDryingType(cols[5]);
//         bool smoothed           = returnSmoothed(cols[6]);
//         std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
//         std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";

//         Slab newSlab(-1, species, thickE, lenQ, widthE, drying, smoothed, location, notes);
//         newSlab.insert();
//     }
// }

// void Importer::importCookies(const std::string& filePath){
//     //id, from_log, species, thickness (quarters), diameter (quarters), drying, location, notes
//     //int, int, string, uint, uint, Drying, string, string
//     std::ifstream file(filePath);
//     if(!file){
//         std::cout << "File could not open at: " << filePath << std::endl;
//         return;
//     }

//     std::string line;
//     if(!std::getline(file, line)){return;}

//     while(std::getline(file, line)){
//         if(line.empty()){continue;}
//         auto cols = digestLine(line);

//         std::string species     = cols[1];
//         uint thickQ             = std::stoul(cols[2]);
//         uint diamQ              = std::stoul(cols[3]);
//         Drying drying           = returnDryingType(cols[4]);
//         std::string location    = (cols.size() > 5 && !cols[5].empty()) ? cols[5] : "";
//         std::string notes       = (cols.size() > 6 && !cols[6].empty()) ? cols[6] : "";

//         Cookie newCookie(-1, -1, species, thickQ, diamQ, drying, location, notes);
//         newCookie.insert();
//     }
// }

// void Importer::importLumber(const std::string& filePath){
//     //id, species, thickness (quarters), length (quarters), width (quarters), drying, surfacing (smoothed), location, notes
//     //int, string, uint, uint, uint, Drying, bool, string, string

//     std::ifstream file(filePath);
//     if(!file){
//         std::cout << "File could not open at: " << filePath << std::endl;
//         return;
//     }

//     std::string line;
//     if(!std::getline(file, line)){return;}

//     while(std::getline(file, line)){
//         if(line.empty()){continue;}
//         auto cols = digestLine(line);

//         std::string species     = cols[1];
//         uint thickQ             = std::stoul(cols[2]);
//         uint lenQ               = std::stoul(cols[3]);
//         uint widthQ             = std::stoul(cols[4]);
//         Drying drying           = returnDryingType(cols[5]);
//         bool smoothed           = returnSmoothed(cols[6]);
//         std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
//         std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";        
        
//         Lumber newLumber(species, thickQ, lenQ, widthQ, drying, smoothed, location, notes);
//         newLumber.insert();
//     }
// }
