#pragma once

#include "parser/Utils.hpp"
#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <string_view>

namespace parser {

class Parser
{
public:
    constexpr Parser(std::string_view content) noexcept
        : content_(content),
          lexer_(content_) {}

    constexpr auto identifier() noexcept
        -> std::optional<ast::Identifier>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            if(token.getType() != lexing::TokenTypes::IDENTIFIER) {
                return std::nullopt;
            }

            lexer_.pop();

            return ast::Identifier{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }

    constexpr auto integer() noexcept
        -> std::optional<ast::Integer>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            if(token.getType() != lexing::TokenTypes::INTEGER) {
                return std::nullopt;
            }

            lexer_.pop();

			auto value = parse_int_unsafe(token.getValue());

            return ast::Integer{token.getArea(), value};
        }

        return std::nullopt;
    }

private:
    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
