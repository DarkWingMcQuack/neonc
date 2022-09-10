#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class IdentifierParser
{
public:
    constexpr IdentifierParser(std::string_view content, lexing::Lexer&& lexer) noexcept
        : content_(content),
          lexer_(std::move(lexer)) {}

    constexpr auto identifier() noexcept
        -> std::optional<ast::Identifier>
    {
        if(lexer_.next_is(lexing::TokenTypes::IDENTIFIER)) {
            auto token = lexer_.peek_and_pop().value();
            return ast::Identifier{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }

    constexpr auto getLexer() -> lexing::Lexer&
    {
        return lexer_;
    }

protected:
    std::string_view content_;
    lexing::Lexer lexer_;
};
} // namespace parser
