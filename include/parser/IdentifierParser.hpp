#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class IdentifierParser
{
public:
    constexpr explicit IdentifierParser(lexing::Lexer& lexer) noexcept
        : lexer_(lexer) {}

    constexpr auto identifier() noexcept
        -> std::optional<ast::Identifier>
    {
        if(lexer_.next_is(lexing::TokenTypes::IDENTIFIER)) {
            auto token = lexer_.peek_and_pop().value();
            return ast::Identifier{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }

protected:
    lexing::Lexer& lexer_;
};
} // namespace parser
