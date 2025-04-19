
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

Drying Importer::returnDryingType(std::string dryStr)
{
    std::transform(dryStr.begin(), dryStr.end(), dryStr.begin(), ::toupper);
    if(dryStr == "AIR DRIED") return AIR_DRIED;
    else if(dryStr == "KILN DRIED") return KILN_DRIED;
    else if(dryStr == "AIR AND KILN DRIED") return AIR_AND_KILN_DRIED;
    else return WET;
}


void Importer::importLogs(const std::string& filePath){
    //id, species, length (quarters), diameter (quarters), cost (cent/quarter), quality, drying, location, notes
    //int, string, uint, uint, uint, int, Drying, string, string
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
        uint lenQuart           = std::stoul(cols[2]);
        uint diamQuart          = std::stoul(cols[3]);
        double cost             = std::stod(cols[4]);
        int quality             = std::stoi(cols[5]);
        Drying drying           = returnDryingType(cols[6]);
        std::string location    = (cols.size() > 7 && !cols[7].empty()) ? cols[7] : "";
        std::string notes       = (cols.size() > 8 && !cols[8].empty()) ? cols[8] : "";

        Log newLog(0, species, lenQuart, diamQuart, cost, quality, drying, location, notes);
        newLog.insert();
    }
}

void Importer::importFirewood(const std::string& filePath){
    //id, species, drying, feet^3, location, notes
    //int, string, Drying, uint, string, string
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
        Drying drying           = returnDryingType(cols[2]);
        uint ft3                = std::stoul(cols[3]);
        std::string location    = (cols.size() > 4 && !cols[4].empty()) ? cols[4] : "";
        std::string notes       = (cols.size() > 5 && !cols[5].empty()) ? cols[5] : "";

        uint taken_lenQ         = 150;

        Firewood newFirewood(0, species, drying, ft3, taken_lenQ, location, notes);
        // from_log attribute failure - pending question
        newFirewood.insert();
    }
}

void Importer::importSlabs(const std::string& filePath){
    //id, species, thickness (eighths), width (eighths), drying, smoothed, location, notes
    //int, string, uint, uint, uint, Drying, bool, string, string
}

void Importer::importCookies(const std::string& filePath){
    //id, species, thickness (quarters), diameter (quarters), drying, location, notes
    //int, string, uint, uint, Drying, string, string

}

void Importer::importLumber(const std::string& filePath){
    //id, species, length (inches), width (quarters), thickness (quarters), drying, surfacing, location, notes
    //int, string, uint, uint, uint, Drying, Surfacing?, string, string

}

/*
void Importer::importProducts(const std::string& filePath){
    //no clue, figure it out after cutlist is finished?

}
*/
