#pragma once
/**
 * @file slabs.hpp
 * @brief Declarations for Slab data‑model and SlabManufacturer helper.
 *
 * Heavyweight definitions (database access, manufacturing, etc.) live in *slabs.cpp*.
 * Lightweight geometry helpers inside SlabManufacturer remain header‑only for inlining.
 */

#include <string>
#include <vector>
#include <optional>
#include "types.hpp"      // Drying, etc.
#include "logs.hpp"       // Log class
#include "interfaces.hpp" // Persistent<>
#include "manufacturable.hpp"

// ---------------------------------------------------------------------------------------------------------------------
//  Slab – Persistable + Manufacturable board cut from a Log
// ---------------------------------------------------------------------------------------------------------------------

class Slab final : public Manufacturable<Slab>
{
public:
    // Construction ------------------------------------------------------------------
    Slab() = default;
    Slab(int                id,
         std::string        species,
         unsigned           thickness_eighths,
         unsigned           len_quarters,
         unsigned           width_eighths,
         Drying             drying          = Drying::WET,
         bool               smoothed        = false,
         std::string        location        = {},
         std::string        notes           = {});

    // Getters -----------------------------------------------------------------------
    [[nodiscard]] int                     get_id()          const noexcept override;
    [[nodiscard]] const std::string&      getSpecies()      const noexcept { return species_; }
    [[nodiscard]] unsigned                getThickness8()   const noexcept { return thickness_eighths_; }
    [[nodiscard]] unsigned                getLenQ()         const noexcept { return len_quarters_; }
    [[nodiscard]] unsigned                getWidth8()       const noexcept { return width_eighths_; }
    [[nodiscard]] Drying                  getDrying()       const noexcept { return drying_; }
    [[nodiscard]] bool                    isSmoothed()      const noexcept { return smoothed_; }
    [[nodiscard]] const std::string&      getLocation()     const noexcept { return location_; }
    [[nodiscard]] const std::string&      getNotes()        const noexcept { return notes_; }

    // Persistent interface ----------------------------------------------------------
    bool insert() override;                      // defined in slabs.cpp
    bool update() override;                      // defined in slabs.cpp
    static std::optional<Slab> get_by_id(int id);        // slabs.cpp
    static std::vector<Slab>   get_all();                // slabs.cpp

    // Manufacturable interface ------------------------------------------------------
    static std::vector<Slab> make_from_log(
        Log                     log,
        int                     len_quarters,
        std::optional<int>      thickness_eighths = std::nullopt,
        std::optional<int>      width_eighths     = std::nullopt,
        std::optional<Drying>   drying            = std::nullopt);   // slabs.cpp

    // Convenience factory used by SlabManufacturer (implemented in slabs.cpp)
    static std::vector<Slab> manufacture_and_persist_slabs(
        Log &                          log,
        const std::vector<struct InProgressSlab>& cuts,
        int                              total_len_quarters,
        const std::string&               location = {},
        const std::string&               notes    = {});

private:
    int          id_{-1};
    std::string  species_;
    unsigned     thickness_eighths_{0};
    unsigned     len_quarters_{0};
    unsigned     width_eighths_{0};
    Drying       drying_{Drying::WET};
    bool         smoothed_{false};
    std::string  location_;
    std::string  notes_;
};

// ---------------------------------------------------------------------------------------------------------------------
//  Helper types for live‑edge slab planning
// ---------------------------------------------------------------------------------------------------------------------

struct InProgressSlab
{
    int width_eighths  = 0;
    int thickness_eighths = 0;
    int kerf_eighths      = 0;
};

// ---------------------------------------------------------------------------------------------------------------------
//  SlabManufacturer – geometry & workflow helper (header‑only)
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
    [[nodiscard]] int                                availableDiameterEighths() const;
    [[nodiscard]] const std::vector<InProgressSlab>& getMadeCuts()            const { return madeCuts_; }
    [[nodiscard]] const std::vector<InProgressSlab>& getRedoQueue()           const { return redoQueue_; }

    // Commit all cuts to real Slab objects and persist
    std::vector<Slab> finalize(const std::string& location = {}, const std::string& notes = {});

private:
    // Geometry helpers (inline) ------------------------------------------------------
    [[nodiscard]] int widthAtOffset(int offset_eighths) const;
    [[nodiscard]] int nextCutWidth(int thickness_eighths) const;

    // State -------------------------------------------------------------------------
    Log*                          log_{nullptr};
    int                           log_len_q_{0};
    int                           log_diameter_8_{0};
    int                           used_diameter_8_{0};

    std::vector<InProgressSlab>   madeCuts_;
    std::vector<InProgressSlab>   redoQueue_;
};

// ---------------------------------------------------------------------------------------------------------------------
//  Inline definitions for SlabManufacturer geometry helpers
// ---------------------------------------------------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>

inline SlabManufacturer::SlabManufacturer(Log &log)
    : log_{&log},
      log_len_q_{static_cast<int>(log.getLenQuarters())},
      log_diameter_8_{static_cast<int>(log.getDiameterQuarters()) * 2},
      used_diameter_8_{0} {}


inline int SlabManufacturer::widthAtOffset(int offset_8) const
{
    const double r = log_diameter_8_ / 2.0;
    const double y = std::abs(r - (used_diameter_8_ + offset_8));
    if (y >= r) return 0;
    return static_cast<int>(2 * std::sqrt(r*r - y*y));
}

inline int SlabManufacturer::nextCutWidth(int thickness_8) const
{
    const int w0 = widthAtOffset(0);
    const int w1 = widthAtOffset(thickness_8);
    return (w0 && w1) ? (w0 + w1) / 2 : 0;
}

inline bool SlabManufacturer::makeSlice(int thickness_8, int kerf_8)
{
    if (thickness_8 <= 0 || thickness_8 + kerf_8 > availableDiameterEighths()) return false;
    const int w = nextCutWidth(thickness_8);
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
    used_diameter_8_ -= madeCuts_.back().thickness_eighths + madeCuts_.back().kerf_eighths;
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

// finalize() is defined in slabs.cpp because it depends on Slab database ops
