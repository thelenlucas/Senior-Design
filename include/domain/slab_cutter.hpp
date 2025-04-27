/**
 * @file slab_cutter.hpp
 * @brief Defines the SlabCutter class and related structures for processing logs into slabs.
 */

#include "log.hpp"
#include "domain/live_edge_slab.hpp"
#include "domain/units.hpp"
#include "domain/types.hpp"
#include "infra/repository.hpp"

#include <vector>
#include <cmath>
#include <optional>
#include <QString>

using namespace std;
using namespace woodworks::infra;

// Helper type
struct InProgressSlab
{
    Length thickness;
    Length width;
};

/**
 * @namespace woodworks::domain::slabs
 * @brief Contains classes and structures related to slab processing.
 */
namespace woodworks::domain::slabs
{
    /**
     * @struct SlabCutter
     * @brief Handles the process of cutting a log into slabs.
     */
    struct SlabCutter
    {
        /**
         * @var SlabCutter::log
         * @brief The source log for the cuts.
         */
        Log log;

        /**
         * @var SlabCutter::plannedCuts
         * @brief A list of slabs that are planned but not yet cut.
         */
        vector<InProgressSlab> plannedCuts;

        /**
         * @var SlabCutter::logDiameterUsed
         * @brief The amount of the log's diameter that has been used.
         */
        Length logDiameterUsed;

        /**
         * @brief Constructs a SlabCutter with the given log.
         * @param sourceLog The log to be processed.
         */
        explicit SlabCutter(const Log &sourceLog) : log(sourceLog), logDiameterUsed(Length::fromTicks(0)) {}

        // Remaining diameter of the log
        Length getRemainingDiameter() const
        {
            return log.diameter - logDiameterUsed;
        }

        /**
         * @brief Wastes a portion of the log, typically to cap the ends.
         * @param amount The amount of the log to waste.
         * @throws std::invalid_argument if the waste amount exceeds the log diameter.
         */
        void waste(Length amount)
        {
            if (amount > log.diameter)
            {
                throw std::invalid_argument("Waste amount is greater than log diameter");
            }
            logDiameterUsed = logDiameterUsed + amount;
        }

        /**
         * @brief Calculates the width of a slab at a given thickness.
         * @param thickness The thickness of the slab.
         * @return The width of the slab at the given thickness.
         */
        Length slabWidthAtThickness(Length thickness) const
        {
            if (thickness + logDiameterUsed > log.diameter)
            {
                return getWidthAtOffset(logDiameterUsed);
            }

            // Otherwise, the average of the width at the current used and the width at the current used + thickness
            Length widthAtCurrent = getWidthAtOffset(logDiameterUsed);
            Length widthAtCurrentPlusThickness = getWidthAtOffset(logDiameterUsed + thickness);
            return (widthAtCurrent + widthAtCurrentPlusThickness) / 2;
        }

        /**
         * @brief Adds a slab to the planned cuts.
         * @param thickness The thickness of the slab to add.
         */
        void addSlab(Length thickness)
        {
            if (thickness + logDiameterUsed > log.diameter)
            {
                // Use from now to the diameter of the log
                InProgressSlab slab = {thickness, slabWidthAtThickness(Length::fromFeet(0))};
                plannedCuts.push_back(slab);
                logDiameterUsed = log.diameter;
            }

            // Add the slab to the planned cuts
            InProgressSlab slab = {thickness, slabWidthAtThickness(thickness)};
            plannedCuts.push_back(slab);
            logDiameterUsed = logDiameterUsed + thickness;
        }

        /**
         * @brief Undoes the last planned cut.
         */
        void undoCut()
        {
            if (plannedCuts.empty())
            {
                return;
            }
            // Remove the last cut
            logDiameterUsed = logDiameterUsed - plannedCuts.back().thickness;
            plannedCuts.pop_back();
        }

        /**
         * @brief Clears all planned cuts.
         */
        void clearPlannedCuts()
        {
            plannedCuts.clear();
            logDiameterUsed = Length::fromTicks(0);
        }

        /**
         * @brief Retrieves a list of planned cuts as strings for display.
         * @return A vector of QStrings describing the planned cuts.
         */
        vector<QString> getPlannedCuts() const
        {
            vector<QString> cuts;
            for (const auto &plannedSlab : plannedCuts)
            {
                cuts.push_back("Thickness: " + QString::number(plannedSlab.thickness.toInches()) + " inches, Width: " + QString::number(plannedSlab.width.toInches()) + " inches");
            }
            return cuts;
        }

        /**
         * @brief Completes the planned cuts at a given length.
         * @param cutLength The length of the cuts.
         * @param location Optional location for the slabs.
         * @param notes Optional notes for the slabs.
         * @return A vector of finalized LiveEdgeSlab objects.
         */
        vector<LiveEdgeSlab> completeCuts(Length cutLength, optional<string> location, optional<string> notes)
        {
            // First, calculate the total volume of all slabs cut (approx)
            double totalVolumeInCubed = 0;
            for (const auto &plannedSlab : plannedCuts)
            {
                // Volume = length * width * thickness
                totalVolumeInCubed += cutLength.toFeet() * plannedSlab.width.toFeet() * plannedSlab.thickness.toFeet();
            }

            // That volume gives us a good measure to get the cost
            vector<LiveEdgeSlab> slabs;
            // Make the cut on the log to get the cost
            Dollar cutWorth = log.cut(cutLength);
            int cents = cutWorth.toCents();

            for (auto plannedSlab : plannedCuts)
            {
                // Volume of this slab
                double volume = cutLength.toFeet() * plannedSlab.width.toFeet() * plannedSlab.thickness.toFeet();

                // Create the slab
                LiveEdgeSlab slab = LiveEdgeSlab::uninitialized();
                slab.species = log.species;
                slab.length = cutLength;
                slab.width = plannedSlab.width;
                slab.thickness = plannedSlab.thickness;
                slab.drying = log.drying;
                slab.surfacing = SlabSurfacing::RGH;
                slab.worth = Dollar(static_cast<int>(cents * (volume / totalVolumeInCubed)));

                if (location.has_value())
                {
                    slab.location = location.value();
                }
                if (notes.has_value())
                {
                    slab.notes = notes.value();
                }

                // Cache the slab
                slab.id = Id{QtSqlRepository<LiveEdgeSlab>::spawn().add(slab)};

                slabs.push_back(slab);
            }
            clearPlannedCuts();
            return slabs;
        }

    private:
        /**
         * @brief Calculates the width of a cut parallel to the log centerline at a given offset.
         * @param offset The offset from the log centerline.
         * @return The width of the cut at the given offset.
         */
        Length getWidthAtOffset(Length offset) const
        {
            if (offset > log.length)
            {
                return Length::fromTicks(0);
            }

            // 2 * sqrt(x(D-x))
            return Length::fromTicks(
                2 * sqrt(offset.toTicks() * (log.diameter.toTicks() - offset.toTicks())));
        }
    };
}