
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "csv_importer.hpp"


std::vector<std::string> Importer::digestLine(const std::string& line){
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string part;
    while(std::getline(ss, part, ',')){parts.push_back(part);}
    return parts;
}


void Importer::importLogs(const std::string& filePath){
    //id, species, length (quarters), diameter (quarters), cost (cent/quarter), quality, drying, location, notes
    //int, string, uint, uint, uint, uint, Drying, string, string

}

void Importer::importFirewood(const std::string& filePath){
    //id, species, drying, feet^3, location, notes
    //int, string, Drying, uint, string, string

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
