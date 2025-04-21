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

namespace woodworks::domain::slabs
{
    struct SlabCutter
    {
        Log log;                            // Source log for the cuts
        vector<InProgressSlab> plannedCuts; // Cuts that are planned but not yet made
        Length logDiameterUsed;

        // Create a new cutter from a log
        explicit SlabCutter(const Log &sourceLog) : log(sourceLog), logDiameterUsed(Length::fromTicks(0)) {}

        // Remaining diameter of the log
        Length getRemainingDiameter() const
        {
            return log.diameter - logDiameterUsed;
        }

        // Waste a portion of the log - mostly to cap the ends
        void waste(Length amount)
        {
            if (amount > log.diameter)
            {
                throw std::invalid_argument("Waste amount is greater than log diameter");
            }
            logDiameterUsed = logDiameterUsed + amount;
        }

        // How wide a slab of a given thickness will be if cut right now
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

        // Add a slab to the planned cuts
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

        // Undo a planned cut
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

        // Clear all planned cuts
        void clearPlannedCuts()
        {
            plannedCuts.clear();
            logDiameterUsed = Length::fromTicks(0);
        }

        // Creates a list of qstrings for use in a list view from the planned cuts
        vector<QString> getPlannedCuts() const
        {
            vector<QString> cuts;
            for (const auto &plannedSlab : plannedCuts)
            {
                cuts.push_back("Thickness: " + QString::number(plannedSlab.thickness.toInches()) + " inches, Width: " + QString::number(plannedSlab.width.toInches()) + " inches");
            }
            return cuts;
        }

        // Complete the planned cuts at a given length
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
        // Gives the width of any cut paralell to the log centerline at a given offset
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