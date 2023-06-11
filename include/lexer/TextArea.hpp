#pragma once

#include <cstdint>

namespace lexing {

/**
 * @brief A class to represent a text area in a document.
 *
 * This class represents a span or area in a document, using start and end positions.
 * The positions are represented as unsigned 64-bit integers. The start and end
 * positions are inclusive.
 *
 * Usage Example:
 * @code
 *     TextArea area(0, 10);
 * @endcode
 */
class TextArea
{
public:
    /**
     * @brief Construct a new TextArea object.
     *
     * This constructor creates a TextArea object given a start and an end position.
     *
     * @param start The start position of the text area.
     * @param end The end position of the text area.
     */
    constexpr TextArea(std::uint64_t start, std::uint64_t end) noexcept
        : start_(start), end_(end) {}

    /**
     * @brief Get the start position of the text area.
     *
     * This method returns the start position of the text area.
     *
     * @return The start position as a std::uint64_t.
     */
    constexpr auto getStart() const noexcept -> std::uint64_t
    {
        return start_;
    }

    /**
     * @brief Get the end position of the text area.
     *
     * This method returns the end position of the text area.
     *
     * @return The end position as a std::uint64_t.
     */
    constexpr auto getEnd() const noexcept -> std::uint64_t
    {
        return end_;
    }

    /**
     * @brief Combine two TextArea objects into a new TextArea object.
     *
     * This static method takes two TextArea objects and combines them into a new
     * TextArea that starts at the earliest start position and ends at the latest
     * end position.
     *
     * @param first The first TextArea object.
     * @param second The second TextArea object.
     * @return A new TextArea object that spans from the earliest start to the latest end.
     */
    constexpr static auto combine(TextArea first, TextArea second) noexcept
        -> TextArea
    {
        return TextArea{first.start_ < second.start_ ? first.start_ : second.start_,
                        first.end_ > second.end_ ? first.end_ : second.end_};
    }

private:
    std::uint64_t start_;
    std::uint64_t end_;
};

} // namespace lexing
