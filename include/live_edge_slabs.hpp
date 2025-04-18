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
     Slab(int         id,
          std::string species,
          unsigned    thickness_eighths,
          unsigned    len_quarters,
          unsigned    width_eighths,
          Drying      drying          = Drying::WET,
          bool        smoothed        = false,
          std::string location        = {},
          std::string notes           = {});
 
     // Getters -----------------------------------------------------------------------
     [[nodiscard]] int                get_id()          const noexcept override;
     wwhg::WwhgSlab toWwhg() const;
     [[nodiscard]] const std::string &getSpecies() const noexcept { return species_; }
     [[nodiscard]] unsigned           getThickness8()   const noexcept { return thickness_eighths_; }
     [[nodiscard]] unsigned           getLenQ()         const noexcept { return len_quarters_; }
     [[nodiscard]] unsigned           getWidth8()       const noexcept { return width_eighths_; }
     [[nodiscard]] Drying             getDrying()       const noexcept { return drying_; }
     [[nodiscard]] bool               isSmoothed()      const noexcept { return smoothed_; }
     [[nodiscard]] const std::string& getLocation()     const noexcept { return location_; }
     [[nodiscard]] const std::string& getNotes()        const noexcept { return notes_; }
 
     // Persistent interface ----------------------------------------------------------
     bool insert() override;                      // defined in slabs.cpp
     bool update() override;                      // defined in slabs.cpp
     static std::optional<Slab> get_by_id(int id);  // defined in slabs.cpp
     static std::vector<Slab>   get_all();          // defined in slabs.cpp

     // Persistence for media column
     QPixmap loadPixmap() const override;
     bool savePixmap(const QPixmap& pixmap) const override;
 
     // Manufacturable interface ------------------------------------------------------
     static std::vector<Slab> make_from_log(
         Log                     log,
         int                     len_quarters,
         std::optional<int>      thickness_eighths = std::nullopt,
         std::optional<int>      width_eighths     = std::nullopt,
         std::optional<Drying>   drying            = std::nullopt);  // defined in slabs.cpp
      
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