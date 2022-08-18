#pragma once

#include <cstdint>

namespace lexing {

class TextArea
{

public:
    constexpr TextArea(std::uint64_t start, std::uint64_t end) noexcept
        : start_(start), end_(end) {}

    constexpr auto getStart() const noexcept -> std::uint64_t
    {
        return start_;
    }

    constexpr auto getEnd() const noexcept -> std::uint64_t
    {
        return end_;
    }

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
