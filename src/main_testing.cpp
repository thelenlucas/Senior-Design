#ifdef BUILDING_WOODWORKS_TEST

#include <cassert>
#include <optional>
#include <stdio.h>
#include "domain/log.hpp"
#include "infra/repository.hpp"
#include "infra/unit_of_work.hpp"
#include "infra/connection.hpp"
#include "infra/mappers/log_mapper.hpp"

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;

int main(int argc, char* argv[]) {
    auto& db = DbConnection::instance();
    UnitOfWork uow(db);
    QtSqlRepository<Log> logs(db);

    woodworks::domain::Log log {
        .id = {1},
        .species = {"Oak"},
        .length = woodworks::domain::imperial::Length::fromFeet(10),
        .diameter = woodworks::domain::imperial::Length::fromInches(12),
        .quality = {5},
        .drying = woodworks::domain::types::Drying::KILN_DRIED,
        .location = "Storage",
        .notes = "Test log"
    };

    logs.add(log);
    uow.commit();
}

#endif