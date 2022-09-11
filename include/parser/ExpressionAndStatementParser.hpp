
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
        if(auto result = literal()) {
            return result;
        }

        if(auto result = if_expression()) {
            return std::move(result.value());
        }
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

    constexpr auto block_expression() noexcept -> std::optional<ast::BlockExpr>
    {
        return std::nullopt;
    }

    constexpr auto elif_expression() noexcept -> std::optional<ast::ElifExpr>
    {
        if(not lexer_.next_is(lexing::TokenTypes::ELIF)) {
            return std::nullopt;
        }

        auto start = lexer_.peek_and_pop().value().getArea();

        auto elif_condition_opt = expression();
        if(not elif_condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto elif_condition = std::move(elif_condition_opt.value());

        auto block_expr_opt = block_expression();
        if(not block_expr_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto block_expr = std::move(block_expr_opt.value());

        auto end = ast::getTextArea(block_expr);

        return ast::ElifExpr{lexing::TextArea::combine(start, end),
                             std::move(elif_condition),
                             std::move(block_expr)};
    }

    constexpr auto if_expression() noexcept -> std::optional<ast::Expression>
    {
        if(not lexer_.next_is(lexing::TokenTypes::IF)) {
            return std::nullopt;
        }

        auto start = lexer_.peek_and_pop().value().getArea();

        auto if_condition_opt = expression();
        if(not if_condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto if_condition = std::move(if_condition_opt.value());

        auto if_block_expr_opt = block_expression();
        if(not if_block_expr_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto if_block_expr = std::move(if_block_expr_opt.value());

        std::vector<ast::ElifExpr> elifs;
        while(lexer_.next_is(lexing::TokenTypes::ELIF)) {
            if(auto elif_expr = elif_expression()) {
                elifs.emplace_back(std::move(elif_expr.value()));
            }
        }

        auto else_block_opt = block_expression();
        if(not else_block_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto else_block = std::move(else_block_opt.value());

        auto end = ast::getTextArea(else_block);

        return ast::Expression{
            ast::forward<ast::IfExpr>(lexing::TextArea::combine(start, end),
                                      std::move(if_condition),
                                      std::move(if_block_expr),
                                      std::move(elifs),
                                      std::move(else_block))};
    }
};

} // namespace parser
