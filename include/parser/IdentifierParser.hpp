#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

template<class T>
class IdentifierParser
{
public:
    constexpr auto identifier() noexcept
        -> std::optional<ast::Identifier>
    {
        if(identifier_lexer().next_is(lexing::TokenTypes::IDENTIFIER)) {
            auto token = identifier_lexer().peek_and_pop().value();
            return ast::Identifier{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }

private:
    constexpr auto identifier_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
