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

int main(int argc, char *argv[])
{
    auto &db = DbConnection::instance();
    UnitOfWork uow(db);
    QtSqlRepository<Log> logs(db);

    auto log1 = std::make_shared<Log>();
    log1->id = Id{-1};
    log1->species = Species{"Oak"};
    log1->length = woodworks::domain::imperial::Length::fromFeet(10);
    log1->diameter = woodworks::domain::imperial::Length::fromInches(12);
    log1->quality = Quality{5};
    log1->drying = woodworks::domain::types::Drying::KILN_DRIED;
    log1->cost = Dollar{500};
    log1->location = "Storage";
    log1->notes = "Test log";

    logs.add(*log1);
    uow.commit();
    auto log2 = logs.get(1);
    assert(log2.has_value());

    UnitOfWork uow2(db);
    // Cookie insertion test
    QtSqlRepository<Cookie> cookies(db);
    auto cookie2 = std::make_shared<Cookie>();
    cookie2->id = Id{-1};
    cookie2->species = Species{"Maple"};
    cookie2->length = woodworks::domain::imperial::Length::fromFeet(5);
    cookie2->diameter = woodworks::domain::imperial::Length::fromInches(6);
    cookie2->drying = woodworks::domain::types::Drying::AIR_DRIED;
    cookie2->worth = Dollar{100};
    cookie2->location = "Warehouse";
    cookie2->notes = "Test cookie";

    cookies.add(*cookie2);
    uow2.commit();
    auto cookie3 = cookies.get(1);
    assert(cookie3.has_value());
    assert(cookie3->species.name == "Maple");

    // LiveEdgeSlab insertion test
    UnitOfWork uow3(db);
    QtSqlRepository<LiveEdgeSlab> slabs(db);
    auto slab2 = std::make_shared<LiveEdgeSlab>();
    slab2->id = Id{-1};
    slab2->species = Species{"Walnut"};
    slab2->length = woodworks::domain::imperial::Length::fromFeet(8);
    slab2->width = woodworks::domain::imperial::Length::fromInches(10);
    slab2->thickness = woodworks::domain::imperial::Length::fromInches(2);
    slab2->drying = woodworks::domain::types::Drying::KILN_DRIED;
    slab2->surfacing = SlabSurfacing::RGH;
    slab2->worth = Dollar{300};
    slab2->location = "Shop";
    slab2->notes = "Test slab";

    slabs.add(*slab2);
    uow3.commit();
    auto slab3 = slabs.get(1);
    assert(slab3.has_value());
    assert(slab3->species.name == "Walnut");
    assert(slab3->length.toInches() == 8 * 12);

    // Lumber insertion test
    UnitOfWork uow4(db);
    QtSqlRepository<Lumber> lumbers(db);
    auto lumber2 = std::make_shared<Lumber>();
    lumber2->id = Id{-1};
    lumber2->species = Species{"Pine"};
    lumber2->length = woodworks::domain::imperial::Length::fromFeet(12);
    lumber2->width = woodworks::domain::imperial::Length::fromInches(4);
    lumber2->thickness = woodworks::domain::imperial::Length::fromInches(1);
    lumber2->drying = woodworks::domain::types::Drying::AIR_DRIED;
    lumber2->surfacing = LumberSurfacing::S4S;
    lumber2->worth = Dollar{200};
    lumber2->location = "Storage";
    lumber2->notes = "Test lumber";

    lumbers.add(*lumber2);
    uow4.commit();
    auto lumber3 = lumbers.get(1);
    assert(lumber3.has_value());
    assert(lumber3->species.name == "Pine");
    assert(lumber3->length.toInches() == 12 * 12);
    assert(lumber3->width.toInches() == 4);

    // Firewood insertion test
    UnitOfWork uow5(db);
    QtSqlRepository<Firewood> firewoods(db);
    auto firewood2 = std::make_shared<Firewood>();
    firewood2->id = Id{-1};
    firewood2->species = Species{"Birch"};
    firewood2->cubicFeet = 10.0;
    firewood2->drying = woodworks::domain::types::Drying::KILN_DRIED;
    firewood2->cost = Dollar{150};
    firewood2->location = "Storage";
    firewood2->notes = "Test firewood";

    firewoods.add(*firewood2);
    uow5.commit();
    auto firewood3 = firewoods.get(1);
    assert(firewood3.has_value());
    assert(firewood3->species.name == "Birch");
    assert(firewood3->cubicFeet == 10.0);
    assert(firewood3->toChords() == 10.0 / 128.0);

    // Create a log and cut a cookie
    UnitOfWork uow6(db);
    auto log3 = std::make_shared<Log>();
    log3->id = Id{-1};
    log3->species = Species{"Oak"};
    log3->length = woodworks::domain::imperial::Length::fromFeet(10);
    log3->diameter = woodworks::domain::imperial::Length::fromInches(12);
    log3->quality = Quality{5};
    log3->drying = woodworks::domain::types::Drying::KILN_DRIED;
    log3->cost = Dollar{5000};
    log3->location = "Storage";
    log3->notes = "Cookie cutting log";

    int id = logs.add(*log3);
    uow6.commit();
    auto log3_2 = logs.get(id).value();
    auto cookie4 = log3_2.cutCookie(woodworks::domain::imperial::Length::fromInches(6));
}

#endif