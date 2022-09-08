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

    constexpr auto boolean() noexcept
        -> std::optional<ast::Boolean>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            switch(token.getType()) {
            case lexing::TokenTypes::TRUE:
                lexer_.pop();
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

    constexpr auto self_type() noexcept
        -> std::optional<ast::Self>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            if(token.getType() != lexing::TokenTypes::SELF_TYPE) {
                return std::nullopt;
            }

            lexer_.pop();

            auto value = parse_int_unsafe(token.getValue());

            return ast::Self{token.getArea()};
        }

        return std::nullopt;
    }

    constexpr auto self_value() noexcept
        -> std::optional<ast::Self>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            if(token.getType() != lexing::TokenTypes::SELF_VALUE) {
                return std::nullopt;
            }

            lexer_.pop();

            auto value = parse_int_unsafe(token.getValue());

            return ast::Self{token.getArea()};
        }

        return std::nullopt;
    }

    constexpr auto string() noexcept
        -> std::optional<ast::String>
    {
        if(auto token_opt = lexer_.peek()) {
            auto token = std::move(token_opt.value());

            if(token.getType() != lexing::TokenTypes::STANDARD_STRING) {
                return std::nullopt;
            }

            lexer_.pop();

            auto value = parse_int_unsafe(token.getValue());

            return ast::String{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }

    constexpr auto named_type() noexcept
        -> std::optional<ast::NamedType>
    {
        std::vector<ast::Identifier> names;

        auto first_identifier_opt = identifier();
        if(not first_identifier_opt) {
            return std::nullopt;
        }

        names.emplace_back(std::move(first_identifier_opt.value()));

        while(lexer_.peek() && lexer_.peek().value().getType() == lexing::TokenTypes::COLON_COLON) {
            lexer_.pop();

            auto identifier_opt = identifier();
            if(not identifier_opt) {
                // TODO: return expexted Identifier error
                return std::nullopt;
            }
            names.emplace_back(std::move(identifier_opt.value()));
        }

        auto area = lexing::TextArea::combine(names.front().getArea(),
                                              names.back().getArea());

        auto type_name = std::move(names.back());
        names.pop_back();

        return ast::NamedType{area, std::move(names), std::move(type_name)};
    }

private:
    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
