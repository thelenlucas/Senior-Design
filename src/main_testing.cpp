#ifdef BUILDING_WOODWORKS_TEST

#include <cassert>
#include <optional>
#include <stdio.h>
#include "domain/log.hpp"
#include "domain/cookie.hpp"
#include "domain/live_edge_slab.hpp"
#include "infra/repository.hpp"
#include "infra/unit_of_work.hpp"
#include "infra/connection.hpp"
#include "infra/mappers/log_mapper.hpp"
#include "infra/mappers/cookie_mapper.hpp"
#include "infra/mappers/live_edge_slab_mapper.hpp"

using namespace woodworks::domain;
using namespace woodworks::domain::imperial;
using namespace woodworks::domain::types;
using namespace woodworks::infra;

int main(int argc, char* argv[]) {
    auto& db = DbConnection::instance();
    UnitOfWork uow(db);
    QtSqlRepository<Log> logs(db);

    woodworks::domain::Log log {
        .id = {-1},
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
    auto log2 = logs.get(1);
    assert(log2.has_value());

    UnitOfWork uow2(db);
    // Cookie insertion test
    QtSqlRepository<Cookie> cookies(db);
    woodworks::domain::Cookie cookie {
        .id = {-1},
        .species = {"Maple"},
        .length = woodworks::domain::imperial::Length::fromFeet(5),
        .diameter = woodworks::domain::imperial::Length::fromInches(6),
        .drying = woodworks::domain::types::Drying::AIR_DRIED,
        .worth = {100},
        .location = "Warehouse",
        .notes = "Test cookie"
    };
    cookies.add(cookie);
    uow2.commit();
    auto cookie2 = cookies.get(1);
    assert(cookie2.has_value());
    assert(cookie2->species.name == "Maple");

    // LiveEdgeSlab insertion test
    UnitOfWork uow3(db);
    QtSqlRepository<LiveEdgeSlab> slabs(db);
    woodworks::domain::LiveEdgeSlab slab {
        .id = {-1},
        .species = {"Walnut"},
        .length = woodworks::domain::imperial::Length::fromFeet(8),
        .width = woodworks::domain::imperial::Length::fromInches(10),
        .thickness = woodworks::domain::imperial::Length::fromInches(2),
        .drying = woodworks::domain::types::Drying::KILN_DRIED,
        .surfacing = SlabSurfacing::RGH,
        .location = "Shop",
        .notes = "Test slab"
    };
    slabs.add(slab);
    uow3.commit();
    auto slab2 = slabs.get(1);
    assert(slab2.has_value());
    assert(slab2->species.name == "Walnut");
    assert(slab2->length.toInches() == 8 * 12);
}

#endif