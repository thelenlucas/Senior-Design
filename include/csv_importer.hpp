#pragma once

#include <vector>

#include "types.hpp"
#include "logs.hpp"
#include "firewood.hpp"
#include "live_edge_slabs.hpp"
#include "cookies.hpp"
#include "lumber.hpp"
//#include "cutlist.hpp"

class Importer
{
    public:
        void importLogs(const std::string& filePath);
        void importFirewood(const std::string& filePath);
        void importSlabs(const std::string& filePath);
        void importCookies(const std::string& filePath);
        void importLumber(const std::string& filePath);

    private:
        static std::vector<std::string> digestLine(const std::string& line);
        Drying returnDryingType(std::string dryStr);
        static bool returnSmoothed(std::string smd);
};
