#pragma once

#include "firewood.hpp"
#include "infra/repository.hpp"
#include <vector>

namespace woodworks::domain
{

    // A bundle of firewood items grouped by a matching example
    struct FirewoodBundle
    {
        Firewood example;
        std::vector<Firewood> items;

        explicit FirewoodBundle(const Firewood &exampleItem)
            : example(exampleItem) {}

        // Add an item if it matches the example criteria
        void add(const Firewood &fw)
        {
            if (Firewood::matches(fw, example))
            {
                items.push_back(fw);
            }
        }

        // Load all matching items from DB using repository
        static FirewoodBundle fromExample(const Firewood& exampleItem) {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            FirewoodBundle bundle(exampleItem);
            bundle.items = repo.filterByExample(exampleItem);
            return bundle;
        }

        // Total volume (cubic feet) across this bundle
        double totalVolume() const noexcept {
            double sum = 0.0;
            for (const auto& fw : items) sum += fw.cubicFeet;
            return sum;
        }

        // Move up to 'volume' cubic feet to a new location
        void moveVolume(double volume, const std::string& newLocation) {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto& fw : items) {
                if (remaining <= 0) {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining) {
                    fw.location = newLocation;
                    repo.update(fw);
                    remaining -= fw.cubicFeet;
                } else {
                    Firewood moved = fw;
                    moved.cubicFeet = remaining;
                    moved.location = newLocation;
                    repo.add(moved);
                    fw.cubicFeet -= remaining;
                    repo.update(fw);
                    kept.push_back(fw);
                    remaining = 0;
                }
            }
            items = std::move(kept);
        }

        // Delete up to 'volume' cubic feet from this bundle
        void deleteVolume(double volume) {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto& fw : items) {
                if (remaining <= 0) {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining) {
                    repo.remove(fw.id.id);
                    remaining -= fw.cubicFeet;
                } else {
                    fw.cubicFeet -= remaining;
                    repo.update(fw);
                    kept.push_back(fw);
                    remaining = 0;
                }
            }
            items = std::move(kept);
        }

        // Bulk dry up to 'volume' cubic feet to a new drying state
        void dryVolume(double volume, types::Drying newDrying) {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto& fw : items) {
                if (remaining <= 0) {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining) {
                    fw.drying = newDrying;
                    repo.update(fw);
                    remaining -= fw.cubicFeet;
                    kept.push_back(fw);
                } else {
                    // split item: dry a portion and leave remainder
                    Firewood dried = fw;
                    dried.cubicFeet = remaining;
                    dried.drying = newDrying;
                    repo.add(dried);

                    fw.cubicFeet -= remaining;
                    repo.update(fw);
                    kept.push_back(fw);
                    remaining = 0;
                }
            }
            items = std::move(kept);
        }
    };

}
