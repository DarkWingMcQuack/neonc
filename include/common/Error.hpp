#pragma once

#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <variant>
#include <vector>

namespace common::error {

class UnknownToken
{
public:
    constexpr UnknownToken(lexing::TextArea area) noexcept
        : area_(std::move(area)) {}

private:
    lexing::TextArea area_;
};

class UnclosedString
{
public:
    constexpr UnclosedString(lexing::TextArea area) noexcept
        : area_(std::move(area)) {}

private:
    lexing::TextArea area_;
};

class UnexpectedToken
{
public:
    template<class... Exp>
        requires(sizeof...(Exp) >= 1) and (std::same_as<Exp, lexing::TokenTypes> and ...)
    constexpr UnexpectedToken(lexing::TokenTypes actual,
                              lexing::TextArea area,
                              Exp... expected) noexcept
        : area_(std::move(area)),
          actual_(std::move(actual)),
          expected_{std::move(expected)...}
    {}

private:
    lexing::TextArea area_;
    lexing::TokenTypes actual_;
    std::vector<lexing::TokenTypes> expected_;
};

using Error = std::variant<UnknownToken, UnclosedString, UnexpectedToken>;

} // namespace common::error
