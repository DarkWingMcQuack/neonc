#pragma once

#include "common/Error.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <cstdio>
#include <exception>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for parsing if expressions
template<class T>
class IfExpressionParser
{
public:
    constexpr auto if_expression() noexcept -> std::expected<ast::Expression, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        // consume the IF Token
        auto start_res = if_expr_lexer().template expect<TokenTypes::IF>();
        if(not start_res.has_value()) {
            return std::unexpected(std::move(start_res.error()));
        }
        auto start = start_res.value().getArea();

        // consume the ( Token
        if(auto open_res = if_expr_lexer().template expect<TokenTypes::L_PARANTHESIS>();
           not open_res.has_value()) {
            return std::unexpected(std::move(open_res.error()));
        }

        // parse the condition
        auto if_condition_res = static_cast<T *>(this)->expression();
        if(not if_condition_res.has_value()) {
            return if_condition_res;
        }
        auto if_condition = std::move(if_condition_res.value());


        // consume the ) Token
        if(auto close_res = if_expr_lexer().template expect<TokenTypes::R_PARANTHESIS>();
           not close_res.has_value()) {
            return std::unexpected(std::move(close_res.error()));
        }

        // parse the "then" expression
        auto if_block_expr_res = static_cast<T *>(this)->expression();
        if(not if_block_expr_res.has_value()) {
            return if_block_expr_res;
        }
        auto if_block_expr = std::move(if_block_expr_res.value());

        std::vector<ast::ElifExpr> elifs;
        while(if_expr_lexer().next_is(lexing::TokenTypes::ELIF)) {
            auto elif_expr = elif_expression();
            if(not elif_expr.has_value()) {
                return std::unexpected(std::move(elif_expr.error()));
            }

            elifs.emplace_back(std::move(elif_expr.value()));
        }

        // consume the "else" Token
        if(auto else_token_res = if_expr_lexer().template expect<TokenTypes::ELSE>();
           not else_token_res.has_value()) {
            return std::unexpected(std::move(else_token_res.error()));
        }

        // parse the else expression
        auto else_block_res = static_cast<T *>(this)->expression();
        if(not else_block_res.has_value()) {
            return std::unexpected(std::move(else_block_res.error()));
        }
        auto else_block = std::move(else_block_res.value());

        auto end = ast::getTextArea(else_block);

        return ast::Expression{
            ast::forward<ast::IfExpr>(lexing::TextArea::combine(start, end),
                                      std::move(if_condition),
                                      std::move(if_block_expr),
                                      std::move(elifs),
                                      std::move(else_block))};
    }

private:
    constexpr auto elif_expression() noexcept -> std::expected<ast::ElifExpr, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        // consume the "elif" Token
        auto start_res = if_expr_lexer().template expect<TokenTypes::ELIF>();
        if(not start_res.has_value()) {
            return std::unexpected(std::move(start_res.error()));
        }
        auto start = start_res.value().getArea();

        // consume the ( Token
        if(auto open_res = if_expr_lexer().template expect<TokenTypes::L_PARANTHESIS>();
           not open_res.has_value()) {
            return std::unexpected(std::move(open_res.error()));
        }

        auto elif_condition_res = static_cast<T *>(this)->expression();
        if(not elif_condition_res) {
            return std::unexpected(std::move(elif_condition_res.error()));
        }
        auto elif_condition = std::move(elif_condition_res.value());

        // consume the ) Token
        if(auto close_res = if_expr_lexer().template expect<TokenTypes::R_PARANTHESIS>();
           not close_res.has_value()) {
            return std::unexpected(std::move(close_res.error()));
        }


        auto block_expr_res = static_cast<T *>(this)->expression();
        if(not block_expr_res.has_value()) {
            return std::unexpected(std::move(block_expr_res.error()));
        }
        auto block_expr = std::move(block_expr_res.value());

        auto end = ast::getTextArea(block_expr);

        return ast::ElifExpr{lexing::TextArea::combine(start, end),
                             std::move(elif_condition),
                             std::move(block_expr)};
    }

private:
    constexpr auto if_expr_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};

} // namespace parser
