#pragma once
#include <cmath>
#include <string>

namespace woodworks::domain::imperial {

    // A length in imperial units - stored as 1/16th inches internally. 
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
        // Construct from an exact number of sixteenths
        static constexpr Length fromTicks(unsigned int ticks) noexcept
        {
            return fromSixteenths(ticks);
        }

        // Construct by rounding to the nearest 1/16 from a double
        static constexpr Length fromInches(double inches) noexcept
        {
            return fromSixteenths(static_cast<unsigned int>(std::round(inches * 16)));
        }

        // Construct by rounding to the nearest 1/16 from a double foot
        static constexpr Length fromFeet(double feet) noexcept
        {
            return fromInches(feet * 12);
        }

        
        // Number of ticks
        [[nodiscard]] constexpr unsigned int toTicks() const noexcept
        {
            return sixteenths_;
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