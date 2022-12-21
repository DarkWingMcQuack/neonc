#pragma once

#include "common/Error.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <expected>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for simple expressions which are:
// 1. tuple
// 2. lambda expressions
// 3. integers
// 4. doubles
// 5. booleans
// 6. strings
// 7. identifiers
// 8. if-else expressions
// 9. grouped expressions which are expresions surrounded by ()
// 10. block expressions
// 11. TODO: list comprehensions
// 12. TODO: for expressions
template<class T>
class SimpleExpressionParser
{
public:
    // parses tuples, literals, lambda expressions and (expressions)
    constexpr auto simple_expression() noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        if(auto result = integer()) {
            return result.value();
        }

        if(auto result = duoble()) {
            return result.value();
        }

        if(auto result = boolean()) {
            return result.value();
        }

        if(auto result = self_value()) {
            return result.value();
        }

        if(auto result = string()) {
            return result.value();
        }

        if(auto result = static_cast<T*>(this)->identifier_or_simple_lambda()) {
            return std::move(result.value());
        }

        if(auto result = static_cast<T*>(this)->if_expression()) {
            return std::move(result.value());
        }

        if(auto result = static_cast<T*>(this)->block_expression()) {
            return std::move(result.value());
        }

        if(auto result = static_cast<T*>(this)->l_par_expression()) {
            return std::move(result.value());
        }

        auto token_res = simple_expr_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(token_res.error());
        }
        auto token = std::move(token_res.value());


        UnexpectedToken error{token.getType(),
                              token.getArea(),
                              TokenTypes::IDENTIFIER,
                              TokenTypes::DOUBLE,
                              TokenTypes::TRUE,
                              TokenTypes::FALSE,
                              TokenTypes::INTEGER,
                              TokenTypes::STANDARD_STRING,
                              TokenTypes::SELF_VALUE,
                              TokenTypes::IF,
                              TokenTypes::L_BRACKET,
                              TokenTypes::L_PARANTHESIS};

        return std::unexpected(std::move(error));
    }

private:
    constexpr auto integer() noexcept -> std::optional<ast::Integer>
    {
        if(not simple_expr_lexer().next_is(lexing::TokenTypes::INTEGER)) {
            return std::nullopt;
        }

        auto token = simple_expr_lexer().peek_and_pop().value();
        auto value = parse_int_unsafe(token.getValue());
        return ast::Integer{token.getArea(), value};
    }

    constexpr auto duoble() noexcept -> std::optional<ast::Double>
    {
        if(not simple_expr_lexer().next_is(lexing::TokenTypes::DOUBLE)) {
            return std::nullopt;
        }

        auto token = simple_expr_lexer().peek_and_pop().value();
        auto value = parse_double_unsafe(token.getValue());
        return ast::Double{token.getArea(), value};
    }

    constexpr auto boolean() noexcept -> std::optional<ast::Boolean>
    {
        if(simple_expr_lexer().next_is(lexing::TokenTypes::TRUE)) {
            auto area = simple_expr_lexer().peek_and_pop().value().getArea();
            return ast::Boolean(area, true);
        }

        if(simple_expr_lexer().next_is(lexing::TokenTypes::FALSE)) {
            auto area = simple_expr_lexer().peek_and_pop().value().getArea();
            return ast::Boolean(area, false);
        }

        return std::nullopt;
    }

    constexpr auto self_value() noexcept -> std::optional<ast::SelfExpr>
    {
        if(simple_expr_lexer().next_is(lexing::TokenTypes::SELF_VALUE)) {
            auto area = simple_expr_lexer().peek_and_pop().value().getArea();
            return ast::SelfExpr{area};
        }

        return std::nullopt;
    }

    constexpr auto string() noexcept -> std::optional<ast::String>
    {
        if(simple_expr_lexer().next_is(lexing::TokenTypes::STANDARD_STRING)) {
            auto token = simple_expr_lexer().peek_and_pop().value();
            return ast::String{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }


private:
    constexpr auto simple_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
