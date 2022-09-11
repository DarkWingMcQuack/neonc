
#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// expression and statements need to be parsed together
// because an blockexpr or a if expr contains statements
// and on the other side statements contain expressions ofc
class ExpressionAndStatementParser : public TypeParser
{
public:
    constexpr explicit ExpressionAndStatementParser(lexing::Lexer& lexer) noexcept
        : TypeParser(lexer) {}

    constexpr auto expression() noexcept -> std::optional<ast::Expression>
    {
        return literal();
    }

    constexpr auto statement() noexcept -> std::optional<ast::Statement>;

private:
    constexpr auto literal() noexcept -> std::optional<ast::Expression>
    {
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

        return std::nullopt;
    }

    constexpr auto integer() noexcept -> std::optional<ast::Integer>
    {
        if(not lexer_.next_is(lexing::TokenTypes::INTEGER)) {
            return std::nullopt;
        }

        auto token = lexer_.peek_and_pop().value();
        auto value = parse_int_unsafe(token.getValue());
        return ast::Integer{token.getArea(), value};
    }

    constexpr auto duoble() noexcept -> std::optional<ast::Double>
    {
        if(not lexer_.next_is(lexing::TokenTypes::DOUBLE)) {
            return std::nullopt;
        }

        auto token = lexer_.peek_and_pop().value();
        auto value = parse_double_unsafe(token.getValue());
        return ast::Double{token.getArea(), value};
    }

    constexpr auto boolean() noexcept -> std::optional<ast::Boolean>
    {
        if(lexer_.next_is(lexing::TokenTypes::TRUE)) {
            auto area = lexer_.next_area().value();
            return ast::Boolean(area, true);
        }

        if(lexer_.next_is(lexing::TokenTypes::FALSE)) {
            auto area = lexer_.next_area().value();
            return ast::Boolean(area, false);
        }

        return std::nullopt;
    }

    constexpr auto self_value() noexcept -> std::optional<ast::Self>
    {
        if(lexer_.next_is(lexing::TokenTypes::SELF_VALUE)) {
            auto area = lexer_.peek_and_pop().value().getArea();
            return ast::Self{area};
        }

        return std::nullopt;
    }

    constexpr auto string() noexcept -> std::optional<ast::String>
    {
        if(lexer_.next_is(lexing::TokenTypes::STANDARD_STRING)) {
            auto token = lexer_.peek_and_pop().value();
            return ast::String{token.getArea(), token.getValue()};
        }

        return std::nullopt;
    }
};

} // namespace parser
