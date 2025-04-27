/**
 * @file firewood_bundle.hpp
 * @brief Defines FirewoodBundle for grouping and managing collections of Firewood items.
 */

#pragma once

#include "firewood.hpp"
#include "infra/repository.hpp"
#include <vector>

namespace woodworks::domain
{

    /**
     * @struct FirewoodBundle
     * @brief Represents a bundle of firewood items grouped by a matching example.
     */
    struct FirewoodBundle
    {
        /**
         * @brief Example firewood item used for grouping.
         */
        Firewood example;

        /**
         * @brief List of firewood items in the bundle.
         */
        std::vector<Firewood> items;

        /**
         * @brief Constructs a FirewoodBundle with a given example item.
         * @param exampleItem The example firewood item.
         */
        explicit FirewoodBundle(const Firewood &exampleItem)
            : example(exampleItem) {}

        /**
         * @brief Adds a firewood item to the bundle if it matches the example criteria.
         * @param fw The firewood item to add.
         */
        void add(const Firewood &fw)
        {
            if (Firewood::matches(fw, example))
            {
                items.push_back(fw);
            }
        }

        /**
         * @brief Loads all matching items from the database using a repository.
         * @param exampleItem The example firewood item for filtering.
         * @return A FirewoodBundle containing the matching items.
         */
        static FirewoodBundle fromExample(const Firewood &exampleItem)
        {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            FirewoodBundle bundle(exampleItem);
            bundle.items = repo.filterByExample(exampleItem);
            return bundle;
        }

        /**
         * @brief Calculates the total volume (in cubic feet) of the firewood in the bundle.
         * @return The total volume of the firewood.
         */
        double totalVolume() const noexcept
        {
            double sum = 0.0;
            for (const auto &fw : items)
                sum += fw.cubicFeet;
            return sum;
        }

        /**
         * @brief Moves up to a specified volume of firewood to a new location.
         * @param volume The volume of firewood to move (in cubic feet).
         * @param newLocation The new location for the firewood.
         */
        void moveVolume(double volume, const std::string &newLocation)
        {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto &fw : items)
            {
                if (remaining <= 0)
                {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining)
                {
                    fw.location = newLocation;
                    repo.update(fw);
                    remaining -= fw.cubicFeet;
                }
                else
                {
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

        /**
         * @brief Deletes up to a specified volume of firewood from the bundle.
         * @param volume The volume of firewood to delete (in cubic feet).
         */
        void deleteVolume(double volume)
        {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto &fw : items)
            {
                if (remaining <= 0)
                {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining)
                {
                    repo.remove(fw.id.id);
                    remaining -= fw.cubicFeet;
                }
                else
                {
                    fw.cubicFeet -= remaining;
                    repo.update(fw);
                    kept.push_back(fw);
                    remaining = 0;
                }
            }
            items = std::move(kept);
        }

        /**
         * @brief Dries up to a specified volume of firewood to a new drying state.
         * @param volume The volume of firewood to dry (in cubic feet).
         * @param newDrying The new drying state for the firewood.
         */
        void dryVolume(double volume, types::Drying newDrying)
        {
            auto repo = woodworks::infra::QtSqlRepository<Firewood>::spawn();
            double remaining = volume;
            std::vector<Firewood> kept;
            for (auto &fw : items)
            {
                if (remaining <= 0)
                {
                    kept.push_back(fw);
                    continue;
                }
                if (fw.cubicFeet <= remaining)
                {
                    fw.drying = newDrying;
                    repo.update(fw);
                    remaining -= fw.cubicFeet;
                    kept.push_back(fw);
                }
                else
                {
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
