/**
 * @file units.hpp
 * @brief Defines the Length class for representing lengths in imperial units.
 */

#pragma once
#include <cmath>
#include <string>

/**
 * @namespace woodworks::domain::imperial
 * @brief Contains classes and functions for working with imperial units.
 */
namespace woodworks::domain::imperial {

    /**
     * @class Length
     * @brief Represents a length in imperial units, stored internally as 1/16th inches.
     */
    class Length
    {
    private:
        unsigned int sixteenths_; // Length in 1/16th inches

        // Explicit from 16th
        constexpr explicit Length(unsigned int sixteenths) noexcept : sixteenths_(sixteenths) {}

        // Static
        static constexpr Length fromSixteenths(unsigned int sixteenths) noexcept
        {
            return Length(sixteenths);
        }
        
    public:
        /**
         * @brief Constructs a Length object from a number of ticks (1/16th inches).
         * @param ticks The number of ticks.
         * @return A Length object.
         */
        static constexpr Length fromTicks(unsigned int ticks) noexcept
        {
            return fromSixteenths(ticks);
        }

        /**
         * @brief Constructs a Length object from a number of quarters (1/4th inches).
         * @param quarters The number of quarters.
         * @return A Length object.
         */
        static constexpr Length fromQuarters(double quarters) noexcept
        {
            return fromSixteenths(static_cast<unsigned int>(std::round(quarters * 4)));
        }

        /**
         * @brief Constructs a Length object from a number of inches.
         * @param inches The number of inches.
         * @return A Length object.
         */
        static constexpr Length fromInches(double inches) noexcept
        {
            return fromSixteenths(static_cast<unsigned int>(std::round(inches * 16)));
        }

        /**
         * @brief Constructs a Length object from a number of feet.
         * @param feet The number of feet.
         * @return A Length object.
         */
        static constexpr Length fromFeet(double feet) noexcept
        {
            return fromInches(feet * 12);
        }

        
        // Number of ticks
        [[nodiscard]] constexpr unsigned int toTicks() const noexcept
        {
            return sixteenths_;
        }

        // Number of quarters
        [[nodiscard]] constexpr unsigned int toQuarters() const noexcept
        {
            return sixteenths_ / 4;
        }

        // Exact length in inches
        [[nodiscard]] constexpr double toInches() const noexcept
        {
            return static_cast<double>(sixteenths_) / 16.0;
        }

        // Exact length in feet
        [[nodiscard]] constexpr double toFeet() const noexcept
        {
            return static_cast<double>(sixteenths_) / 192.0;
        }

        // -------------- Arithmatic -------------- //

        // Add/subtract
        [[nodiscard]] constexpr Length operator+(Length other) const noexcept
        {
            return fromSixteenths(sixteenths_ + other.sixteenths_);
        }
        [[nodiscard]] constexpr Length operator-(Length other) const noexcept
        {
            return fromSixteenths(sixteenths_ - other.sixteenths_);
        }
        
        // Scale by a floating point scalar (rounded off to the nearest tick)
        [[nodiscard]] constexpr Length operator*(double scalar) const noexcept
        {
            return fromSixteenths(static_cast<unsigned int>(std::round(sixteenths_ * scalar)));
        }
        [[nodiscard]] constexpr Length operator/(double scalar) const noexcept
        {
            return fromSixteenths(static_cast<unsigned int>(std::round(sixteenths_ / scalar)));
        }

        // Ratio of two lengths
        [[nodiscard]] constexpr double operator/(Length other) const noexcept
        {
            return static_cast<double>(sixteenths_) / static_cast<double>(other.sixteenths_);
        }

        // Comparison
        constexpr bool operator==(Length other) const noexcept
        {
            return sixteenths_ == other.sixteenths_;
        }
        constexpr bool operator!=(Length other) const noexcept
        {
            return sixteenths_ != other.sixteenths_;
        }
        constexpr bool operator<(Length other) const noexcept
        {
            return sixteenths_ < other.sixteenths_;
        }
        constexpr bool operator<=(Length other) const noexcept
        {
            return sixteenths_ <= other.sixteenths_;
        }
        constexpr bool operator>(Length other) const noexcept
        {
            return sixteenths_ > other.sixteenths_;
        }
        constexpr bool operator>=(Length other) const noexcept
        {
            return sixteenths_ >= other.sixteenths_;
        }

        // Formats to a string
        [[nodiscard]] std::string toString() const noexcept
        {
            unsigned int inches = sixteenths_ / 16;
            unsigned int remainder = sixteenths_ % 16;
            return std::to_string(inches) + "." + std::to_string(remainder);
        }
        [[nodiscard]] std::string toFeetString() const noexcept
        {
            unsigned int feet = sixteenths_ / 192;
            unsigned int remainder = sixteenths_ % 192;
            return std::to_string(feet) + "." + std::to_string(remainder);
        }
        [[nodiscard]] std::string toInchesString() const noexcept
        {
            unsigned int inches = sixteenths_ / 16;
            unsigned int remainder = sixteenths_ % 16;
            return std::to_string(inches) + "." + std::to_string(remainder);
        }
    };
}