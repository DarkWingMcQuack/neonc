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


constexpr auto parse_int(std::string_view str) noexcept
    -> std::optional<std::int64_t>
{
    if(str.empty()) [[unlikely]] {
        return std::nullopt;
    }

    std::int64_t value = 0;
    for(char c : str) {
        if(!is_digit(c)) [[unlikely]] {
            return std::nullopt;
        }
        value = (c - '0') + value * 10;
    }

    return value;
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

} // namespace parser
