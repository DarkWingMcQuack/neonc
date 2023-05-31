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
    double result = 0.0;
    std::size_t index = 0;

    // Parse integer part
    while(index < str.size() && std::isdigit(str[index])) {
        result = result * 10 + (str[index++] - '0');
    }

    if(index < str.size() && str[index] == '.') {
        double fraction = 1.0;
        ++index;

        // Parse fractional part
        while(index < str.size() && std::isdigit(str[index])) {
            fraction /= 10.0;
            result += (str[index++] - '0') * fraction;
        }
    }

    if(index < str.size() && (str[index] == 'e' || str[index] == 'E')) {
        ++index;

        double base = 10.0;
        if(index < str.size()) {
            if(str[index] == '-') {
                base = 0.1;
                ++index;
            } else if(str[index] == '+') {
                ++index;
            }
        }

        unsigned exponent = 0;
        while(index < str.size() && std::isdigit(str[index])) {
            exponent = exponent * 10 + (str[index++] - '0');
        }

        while(exponent--) {
            result *= base;
        }
    }

    return result;
}

} // namespace parser
