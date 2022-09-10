#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class Parser
{
public:
    constexpr Parser(std::string_view content) noexcept
        : content_(content),
          lexer_(content_) {}


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

    constexpr auto boolean() noexcept
        -> std::optional<ast::Boolean>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            switch(token.getType()) {
            case lexing::TokenTypes::TRUE: lexer_.pop();
                return ast::Boolean(token.getArea(), true);

            case lexing::TokenTypes::FALSE:
                lexer_.pop();
                return ast::Boolean(token.getArea(), false);

            default:
                return std::nullopt;
            }
        }

        return std::nullopt;
    }


    constexpr auto self_value() noexcept
        -> std::optional<ast::Self>
    {
        if(lexer_.next_is(lexing::TokenTypes::SELF_VALUE)) {
            auto area = lexer_.peek_and_pop().value().getArea();
            return ast::Self{area};
        }

        return std::nullopt;
    }

    constexpr auto string() noexcept
        -> std::optional<ast::String>
    {
        if(lexer_.next_is(lexing::TokenTypes::STANDARD_STRING)) {
            auto token = lexer_.peek_and_pop().value();
            return ast::String{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }


private:
    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
