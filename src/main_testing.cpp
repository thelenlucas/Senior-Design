#ifdef BUILDING_WOODWORKS_TEST

#include <cassert>
#include <optional>
#include <stdio.h>
#include "domain/log.hpp"
#include "domain/cookie.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/lumber.hpp"
#include "infra/repository.hpp"
#include "infra/unit_of_work.hpp"
#include "infra/connection.hpp"
#include "infra/mappers/log_mapper.hpp"
#include "infra/mappers/cookie_mapper.hpp"
#include "infra/mappers/live_edge_slab_mapper.hpp"
#include "infra/mappers/lumber_mapper.hpp"

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
        .cost = {500},
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
        .worth = {300},
        .location = "Shop",
        .notes = "Test slab"
    };
    slabs.add(slab);
    uow3.commit();
    auto slab2 = slabs.get(1);
    assert(slab2.has_value());
    assert(slab2->species.name == "Walnut");
    assert(slab2->length.toInches() == 8 * 12);

    // Lumber insertion test
    UnitOfWork uow4(db);
    QtSqlRepository<Lumber> lumbers(db);
    woodworks::domain::Lumber lumber {
        .id = {-1},
        .species = {"Pine"},
        .length = woodworks::domain::imperial::Length::fromFeet(12),
        .width = woodworks::domain::imperial::Length::fromInches(4),
        .thickness = woodworks::domain::imperial::Length::fromInches(1),
        .drying = woodworks::domain::types::Drying::AIR_DRIED,
        .surfacing = LumberSurfacing::S4S,
        .worth = {200},
        .location = "Storage",
        .notes = "Test lumber"
    };
    lumbers.add(lumber);
    uow4.commit();
    auto lumber2 = lumbers.get(1);
    assert(lumber2.has_value());
    assert(lumber2->species.name == "Pine");
    assert(lumber2->length.toInches() == 12 * 12);
    assert(lumber2->width.toInches() == 4);

    // Firewood insertion test
    UnitOfWork uow5(db);
    QtSqlRepository<Firewood> firewoods(db);
    woodworks::domain::Firewood firewood {
        .id = {-1},
        .species = {"Birch"},
        .cubicFeet = 10.0,
        .drying = woodworks::domain::types::Drying::KILN_DRIED,
        .cost = {150},
        .location = "Storage",
        .notes = "Test firewood"
    };
    firewoods.add(firewood);
    uow5.commit();
    auto firewood2 = firewoods.get(1);
    assert(firewood2.has_value());
    assert(firewood2->species.name == "Birch");
    assert(firewood2->cubicFeet == 10.0);
    assert(firewood2->toChords() == 10.0 / 128.0);

    // Create a log and cut a cookie
    UnitOfWork uow6(db);
    woodworks::domain::Log log3 {
        .id = {-1},
        .species = {"Oak"},
        .length = woodworks::domain::imperial::Length::fromFeet(10),
        .diameter = woodworks::domain::imperial::Length::fromInches(12),
        .quality = {5},
        .drying = woodworks::domain::types::Drying::KILN_DRIED,
        .cost = {5000},
        .location = "Storage",
        .notes = "Cookie cutting log"
    };
    int id = logs.add(log3);
    uow6.commit();
    auto log3_2 = logs.get(id).value();
    auto cookie3 = log3_2.cutCookie(woodworks::domain::imperial::Length::fromInches(6));
}

#endif