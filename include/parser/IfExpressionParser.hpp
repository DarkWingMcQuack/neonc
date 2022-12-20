#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for parsing if expressions
template<class T>
class IfExpressionParser
{
public:
    constexpr auto if_expression() noexcept -> std::optional<ast::Expression>
    {
        if(not if_expr_lexer().next_is(lexing::TokenTypes::IF)) {
            return std::nullopt;
        }

        auto start = if_expr_lexer().peek_and_pop().value().getArea();

        if(not if_expr_lexer().pop_next_is(lexing::TokenTypes::L_PARANTHESIS)) {
            // TODO: return error "expected ("
            return std::nullopt;
        }

        auto if_condition_opt = static_cast<T*>(this)->expression();
        if(not if_condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto if_condition = std::move(if_condition_opt.value());


        if(not if_expr_lexer().pop_next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // TODO: return error "expected )"
            return std::nullopt;
        }

        auto if_block_expr_opt = static_cast<T*>(this)->expression();
        if(not if_block_expr_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto if_block_expr = std::move(if_block_expr_opt.value());

        std::vector<ast::ElifExpr> elifs;
        while(if_expr_lexer().next_is(lexing::TokenTypes::ELIF)) {
            auto elif_expr = elif_expression();

            if(not elif_expr.has_value()) {
                // TODO: propagate error
                return std::nullopt;
            }

            elifs.emplace_back(std::move(elif_expr.value()));
        }

        if(not if_expr_lexer().pop_next_is(lexing::TokenTypes::ELSE)) {
            // TODO: reutrn "exprected else keyword here"

            return std::nullopt;
        }

        auto else_block_opt = static_cast<T*>(this)->expression();
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

private:
    constexpr auto elif_expression() noexcept -> std::optional<ast::ElifExpr>
    {
        if(not if_expr_lexer().next_is(lexing::TokenTypes::ELIF)) {
            return std::nullopt;
        }

        auto start = if_expr_lexer().peek_and_pop().value().getArea();

        if(not if_expr_lexer().pop_next_is(lexing::TokenTypes::L_PARANTHESIS)) {
            // TODO: return error "expected ("
            return std::nullopt;
        }

        auto elif_condition_opt = static_cast<T*>(this)->expression();
        if(not elif_condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto elif_condition = std::move(elif_condition_opt.value());

        if(not if_expr_lexer().pop_next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // TODO: return error "expected )"
            return std::nullopt;
        }

        auto block_expr_opt = static_cast<T*>(this)->expression();
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

private:
    constexpr auto if_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
