#pragma once
#include <stdexcept>
#include <stdint.h>
#include <string>

namespace woodworks::domain::types {
    // Species
    struct Species {std::string name; };

    // IDs
    struct Id {
        int id;

        constexpr Id uninitialized() const noexcept { return Id{-1}; }
        constexpr bool isUninitialized() const noexcept { return id == -1; }
    };

    // Dollar value in cents
    struct Dollar {
        int cents;

        constexpr Dollar worthless() const noexcept { return Dollar{0}; }

        std::string format() const noexcept
        {
            return "$" + std::to_string(cents / 100) + "." + std::to_string(cents % 100);
        }
    };

    // Quality - 1-5 allowed
    struct Quality {
        int value;

        constexpr Quality(int v) : value(v) {
            if (v < 1 || v > 5)
                throw std::invalid_argument("Quality must be between 1 and 5");
        }
        constexpr Quality uninitialized() const noexcept { return Quality{-1}; }
        constexpr bool isUninitialized() const noexcept { return value == -1; }
        constexpr bool isValid() const noexcept { return value >= 1 && value <= 5; }
    };

    // Enumeration for drying states
    enum class Drying : uint8_t
    {
        GREEN,
        KILN_DRIED,
        AIR_DRIED,
        KILN_AND_AIR_DRIED,
    };

    // Enumeration for live edge surfacing states
    enum class SlabSurfacing : uint8_t
    {
        RGH,
        S1S,
        S2S,
    };

    // Enumeration for lumber surfacing states
    enum class LumberSurfacing : uint8_t
    {
        RGH,
        S1S,
        S2S,
        S3S,
        S4S
    };

    // Convert from live-edge to lumber surfacing
    LumberSurfacing toLumberSurfacing(SlabSurfacing surf)
    {
        switch (surf)
        {
            case SlabSurfacing::RGH: return LumberSurfacing::RGH;
            case SlabSurfacing::S1S: return LumberSurfacing::S1S;
            case SlabSurfacing::S2S: return LumberSurfacing::S2S;
        }
    }

    // Formatting for drying
    std::string toString(Drying drying)
    {
        switch (drying)
        {
            case Drying::GREEN: return "Green";
            case Drying::KILN_DRIED: return "Kiln Dried";
            case Drying::AIR_DRIED: return "Air Dried";
            case Drying::KILN_AND_AIR_DRIED: return "Kiln and Air Dried";
        }
        throw std::invalid_argument("Invalid drying state");
    }

    // Formatting for slab surfacing
    std::string toString(SlabSurfacing surf)
    {
        switch (surf)
        {
            case SlabSurfacing::RGH: return "RGH";
            case SlabSurfacing::S1S: return "S1S";
            case SlabSurfacing::S2S: return "S2S";
        }
        throw std::invalid_argument("Invalid surfacing state");
    }

    // Formatting for lumber surfacing
    std::string toString(LumberSurfacing surf)
    {
        switch (surf)
        {
            case LumberSurfacing::RGH: return "RGH";
            case LumberSurfacing::S1S: return "S1S";
            case LumberSurfacing::S2S: return "S2S";
            case LumberSurfacing::S3S: return "S3S";
            case LumberSurfacing::S4S: return "S4S";
        }
        throw std::invalid_argument("Invalid surfacing state");
    }
}