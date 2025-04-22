#pragma once

#include <vector>
#include "domain/types.hpp"
#include "domain/units.hpp"
#include "domain/log.hpp"
#include "domain/firewood.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/cookie.hpp"
#include "domain/lumber.hpp"
#include "infra/connection.hpp"
#include "infra/repository.hpp"

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
        woodworks::domain::types::Drying returnDryingType(std::string dryStr);
        uint returnSmoothed(std::string smd);
};
