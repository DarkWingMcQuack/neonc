#pragma once

#include <cstdlib>
#include <optional>
#include <string_view>

namespace parser {

constexpr auto is_digit(char c) noexcept
    -> bool
{
    return c <= '9' && c >= '0';
}

constexpr auto parse_int_unsafe(std::string_view str) noexcept
    -> std::int64_t
{
    std::int64_t value = 0;
    for(char c : str) {
        value = (c - '0') + value * 10;
    }

    return value;
}

constexpr auto parse_double_unsafe(std::string_view str) noexcept
    -> double
{
    // TODO: implement double parsing
    return 0.0;
}

} // namespace parser
