/**
 * @file types.hpp
 * @brief Defines various types and enumerations used in the woodworks domain.
 */

#pragma once
#include <stdexcept>
#include <stdint.h>
#include <string>

#include <vector>

#include <QMetaType>

namespace woodworks::domain::types
{
    /**
     * @namespace woodworks::domain::types
     * @brief Contains types and enumerations for species, IDs, dollar values, quality, and surfacing states.
     */

    // Species
    struct Species {std::string name; };

    /**
     * @struct Id
     * @brief Represents a unique identifier for objects.
     */

    struct Id {
        int id;

        constexpr Id uninitialized() const noexcept { return Id{-1}; }
        constexpr bool isUninitialized() const noexcept { return id == -1; }
    };

    /**
     * @struct Dollar
     * @brief Represents a monetary value in cents.
     */

    struct Dollar {
        int cents;

        constexpr Dollar worthless() const noexcept { return Dollar{0}; }

        static constexpr Dollar fromCents(int c) noexcept { return Dollar{c}; }

        /**
         * @brief Constructs a Dollar object.
         * @param c The monetary value in cents.
         * @throws std::invalid_argument if the value is negative.
         */

        constexpr Dollar(int c) : cents(c) {
            if (c < 0)
                throw std::invalid_argument("Dollar value cannot be negative");
        }

        constexpr unsigned int toCents() const noexcept { return static_cast<unsigned int>(cents); }

        std::string format() const noexcept
        {
            return "$" + std::to_string(cents / 100) + "." + std::to_string(cents % 100);
        }
    };

    // Quality - 1-5 allowed
    struct Quality {
        int value;

        /**
         * @brief Constructs a Quality object.
         * @param v The quality value.
         * @throws std::invalid_argument if the value is not between 1 and 5 or is not -1 (uninitialized).
         */

        constexpr Quality(int v) : value(v) {
            if ((v < 1 || v > 5) && v != -1) // Allowing -1 for uninitialized
                throw std::invalid_argument("Quality must be between 1 and 5: attempted quality" + std::to_string(v));
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

    /**
     * @brief Returns the allowed transitions for a given drying state.
     * @param drying The current drying state.
     * @return A vector of allowed drying states.
     */

    inline std::vector<Drying> allowedTransitions(Drying drying) {
        switch (drying)
        {
            case Drying::GREEN:
                return {Drying::KILN_DRIED, Drying::AIR_DRIED};
            case Drying::KILN_DRIED:
                return {Drying::GREEN, Drying::AIR_DRIED, Drying::KILN_AND_AIR_DRIED};
            case Drying::AIR_DRIED:
                return {Drying::GREEN, Drying::KILN_DRIED, Drying::KILN_AND_AIR_DRIED};
            case Drying::KILN_AND_AIR_DRIED:
                return {Drying::GREEN, Drying::KILN_DRIED, Drying::AIR_DRIED};
        }
        throw std::invalid_argument("Invalid drying state");
    }

    // Enumeration for live edge surfacing states
    enum class SlabSurfacing : uint8_t
    {
        RGH,
        S1S,
        S2S,
    };

    /**
     * @brief Returns the allowed transitions for a given slab surfacing state.
     * @param surf The current slab surfacing state.
     * @return A vector of allowed slab surfacing states.
     */

    inline std::vector<SlabSurfacing> allowedTransitions(SlabSurfacing surf) {
        switch (surf)
        {
            case SlabSurfacing::RGH:
                return {SlabSurfacing::S1S, SlabSurfacing::S2S};
            case SlabSurfacing::S1S:
                return {SlabSurfacing::S2S};
            case SlabSurfacing::S2S:
                return {SlabSurfacing::S2S};
        }
        throw std::invalid_argument("Invalid surfacing state");
    }

    // Enumeration for lumber surfacing states
    enum class LumberSurfacing : uint8_t
    {
        RGH,
        S1S,
        S2S,
        S3S,
        S4S
    };

    /**
     * @brief Returns the allowed transitions for a given lumber surfacing state.
     * @param surf The current lumber surfacing state.
     * @return A vector of allowed lumber surfacing states.
     */

    inline std::vector<LumberSurfacing> allowedTransitions(LumberSurfacing surf) {
        switch (surf)
        {
            case LumberSurfacing::RGH:
                return {LumberSurfacing::S1S, LumberSurfacing::S2S, LumberSurfacing::S3S, LumberSurfacing::S4S};
            case LumberSurfacing::S1S:
                return {LumberSurfacing::S2S, LumberSurfacing::S3S, LumberSurfacing::S4S};
            case LumberSurfacing::S2S:
                return {LumberSurfacing::S3S, LumberSurfacing::S4S};
            case LumberSurfacing::S3S:
                return {LumberSurfacing::S4S};
            case LumberSurfacing::S4S:
                return {LumberSurfacing::S4S};
        }
        throw std::invalid_argument("Invalid surfacing state");
    }

    /**
     * @brief Converts a slab surfacing state to a lumber surfacing state.
     * @param surf The slab surfacing state.
     * @return The corresponding lumber surfacing state.
     */

    inline LumberSurfacing toLumberSurfacing(SlabSurfacing surf)
    {
        switch (surf)
        {
            case SlabSurfacing::RGH: return LumberSurfacing::RGH;
            case SlabSurfacing::S1S: return LumberSurfacing::S1S;
            case SlabSurfacing::S2S: return LumberSurfacing::S2S;
        }
        throw std::invalid_argument("Invalid surfacing state");
    }

    /**
     * @brief Converts a drying state to a string representation.
     * @param drying The drying state.
     * @return A string representation of the drying state.
     */

    inline std::string toString(Drying drying)
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

    /**
     * @brief Converts a slab surfacing state to a string representation.
     * @param surf The slab surfacing state.
     * @return A string representation of the slab surfacing state.
     */

    inline std::string toString(SlabSurfacing surf)
    {
        switch (surf)
        {
            case SlabSurfacing::RGH: return "RGH";
            case SlabSurfacing::S1S: return "S1S";
            case SlabSurfacing::S2S: return "S2S";
        }
        throw std::invalid_argument("Invalid surfacing state");
    }

    /**
     * @brief Converts a lumber surfacing state to a string representation.
     * @param surf The lumber surfacing state.
     * @return A string representation of the lumber surfacing state.
     */

    inline std::string toString(LumberSurfacing surf)
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

Q_DECLARE_METATYPE(woodworks::domain::types::Drying);
Q_DECLARE_METATYPE(woodworks::domain::types::SlabSurfacing);
Q_DECLARE_METATYPE(woodworks::domain::types::LumberSurfacing);