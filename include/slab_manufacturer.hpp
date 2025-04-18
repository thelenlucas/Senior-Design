#pragma once
/**
 * @file slab_manufacturer.hpp
 * @brief Declarations for SlabManufacturer – geometry & workflow helper.
 *
 * Provides helper functions for live‑edge slab planning and manufacturing.
 */

#include "live_edge_slabs.hpp"   // Include the Slab data‑model (defines Slab)
#include "logs.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include "multi_cut.hpp"

// ---------------------------------------------------------------------------------------------------------------------
//  Helper types for live‑edge slab planning
// ---------------------------------------------------------------------------------------------------------------------
struct InProgressSlab
{
    int width_eighths    = 0;
    int thickness_eighths = 0;
    int kerf_eighths      = 0;
};

// ---------------------------------------------------------------------------------------------------------------------
//  SlabManufacturer – geometry & workflow helper
// ---------------------------------------------------------------------------------------------------------------------
class SlabManufacturer
{
public:
    explicit SlabManufacturer(Log &log);

    // Plan / modify cuts -------------------------------------------------------------
    bool makeSlice(int thickness_eighths, int kerf_eighths = 0);
    bool undo();
    bool redo();

    // Introspection ------------------------------------------------------------------
    [[nodiscard]] int                                 availableDiameterEighths() const;
    [[nodiscard]] const std::vector<InProgressSlab>&  getMadeCuts()  const { return madeCuts_; }
    [[nodiscard]] const std::vector<InProgressSlab>&  getRedoQueue() const { return redoQueue_; }

    // Commit all cuts to real Slab objects and persist (defined in slabs.cpp)
    std::vector<Slab> finalize(const unsigned int len_quarters, const std::string& location = {}, const std::string& notes = {}) {
        // Get the number of cuts made
        const int numCuts = static_cast<int>(madeCuts_.size());
        if (numCuts == 0) return {};

        // Create a vector of Slab objects
        std::vector<Slab> slabs;
        slabs.reserve(numCuts);
        for (const auto& cut : madeCuts_) {
            Slab slab = Slab::make_from_log(
                *log_,
                len_quarters,
                cut.thickness_eighths,
                cut.width_eighths,
                log_->getDrying()
            )[0];

            slabs.push_back(slab);
        }

        // Create a multicut for the slabs
        MultiCut multiCut(log_->get_id(), log_len_q_, log_diameter_8_, location, numCuts);
        multiCut.insert();

        return slabs;
    }

private:
    // Geometry helpers (inline) ------------------------------------------------------
    [[nodiscard]] int widthAtOffset(int offset_eighths) const;
    [[nodiscard]] int nextCutWidth(int thickness_eighths) const;

    // State -------------------------------------------------------------------------
    Log*                        log_{nullptr};
    int                         log_len_q_{0};
    int                         log_diameter_8_{0};
    int                         used_diameter_8_{0};

    std::vector<InProgressSlab> madeCuts_;
    std::vector<InProgressSlab> redoQueue_;
};

// ---------------------------------------------------------------------------------------------------------------------
//  Inline definitions for SlabManufacturer geometry helpers
// ---------------------------------------------------------------------------------------------------------------------
inline SlabManufacturer::SlabManufacturer(Log &log)
    : log_{&log},
      log_len_q_{static_cast<int>(log.getLenQuarters())},
      log_diameter_8_{static_cast<int>(log.getDiameterQuarters()) * 2},
      used_diameter_8_{0}
{}

inline int SlabManufacturer::widthAtOffset(int offset_8) const
{
    const double r = log_diameter_8_ / 2.0;
    const double y = std::abs(r - (used_diameter_8_ + offset_8));
    if (y >= r) return 0;
    return static_cast<int>(2 * std::sqrt(r * r - y * y));
}

inline int SlabManufacturer::nextCutWidth(int thickness_8) const
{
    const int w0 = widthAtOffset(this->used_diameter_8_);
    const int w1 = widthAtOffset(this->used_diameter_8_ + thickness_8);
    // Average of the two widths
    const int w = (w0 + w1) / 2;
    // Round to the nearest 1/8 inch, rounding up
    const int w_8 = static_cast<int>(std::ceil(w / 8.0)) * 8;
    return w_8;
}

inline bool SlabManufacturer::makeSlice(int thickness_8, int kerf_8)
{
    if (thickness_8 + kerf_8 >= availableDiameterEighths()) return false;
    const int w = nextCutWidth(thickness_8);
    #ifdef BUILDING_WOODWORKS_TEST
    std::cout << "Width: " << w << std::endl;
    #endif
    if (w == 0) return false;
    madeCuts_.push_back({w, thickness_8, kerf_8});
    used_diameter_8_ += thickness_8 + kerf_8;
    redoQueue_.clear();
    return true;
}

inline bool SlabManufacturer::undo()
{
    if (madeCuts_.empty()) return false;
    redoQueue_.push_back(madeCuts_.back());
    used_diameter_8_ -= (madeCuts_.back().thickness_eighths + madeCuts_.back().kerf_eighths);
    madeCuts_.pop_back();
    return true;
}

inline bool SlabManufacturer::redo()
{
    if (redoQueue_.empty()) return false;
    const auto cut = redoQueue_.back();
    if (cut.thickness_eighths + cut.kerf_eighths > availableDiameterEighths()) return false;
    madeCuts_.push_back(cut);
    used_diameter_8_ += cut.thickness_eighths + cut.kerf_eighths;
    redoQueue_.pop_back();
    return true;
}

inline int SlabManufacturer::availableDiameterEighths() const
{
    return log_diameter_8_ - used_diameter_8_;
}