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

        auto start_token_res = if_expr_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());

        if(start_token.getType() != lexing::TokenTypes::IF) {
            UnexpectedToken error{start_token.getType(),
                                  start_token.getArea(),
                                  TokenTypes::IF};

            return std::unexpected(std::move(error));
        }

        auto start = start_token.getArea();


        // consume the ( Token
        auto open_token_res = if_expr_lexer().peek_and_pop();
        if(not open_token_res.has_value()) {
            return std::unexpected(std::move(open_token_res.error()));
        }
        auto open_token = std::move(open_token_res.value());

        if(open_token.getType() != lexing::TokenTypes::L_PARANTHESIS) {
            UnexpectedToken error{open_token.getType(),
                                  open_token.getArea(),
                                  TokenTypes::L_PARANTHESIS};

            return std::unexpected(std::move(error));
        }

        // parse the condition
        auto if_condition_res = static_cast<T*>(this)->expression();
        if(not if_condition_res.has_value()) {
            return if_condition_res;
        }
        auto if_condition = std::move(if_condition_res.value());


        // consume the ) Token
        auto close_token_res = if_expr_lexer().peek_and_pop();
        if(not close_token_res.has_value()) {
            return std::unexpected(std::move(close_token_res.error()));
        }
        auto close_token = std::move(close_token_res.value());

        if(close_token.getType() != lexing::TokenTypes::R_PARANTHESIS) {
            UnexpectedToken error{close_token.getType(),
                                  close_token.getArea(),
                                  TokenTypes::R_PARANTHESIS};

            return std::unexpected(std::move(error));
        }


        // parse the "then" expression
        auto if_block_expr_res = static_cast<T*>(this)->expression();
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
        auto else_token_res = if_expr_lexer().peek_and_pop();
        if(not else_token_res.has_value()) {
            return std::unexpected(std::move(else_token_res.error()));
        }
        auto else_token = std::move(else_token_res.value());

        if(else_token.getType() != lexing::TokenTypes::ELSE) {
            UnexpectedToken error{else_token.getType(),
                                  else_token.getArea(),
                                  TokenTypes::ELSE};

            return std::unexpected(std::move(error));
        }

        auto else_block_res = static_cast<T*>(this)->expression();
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
        auto elif_token_res = if_expr_lexer().peek_and_pop();
        if(not elif_token_res.has_value()) {
            return std::unexpected(std::move(elif_token_res.error()));
        }
        auto elif_token = std::move(elif_token_res.value());

        if(elif_token.getType() != lexing::TokenTypes::ELIF) {
            UnexpectedToken error{elif_token.getType(),
                                  elif_token.getArea(),
                                  TokenTypes::ELIF};

            return std::unexpected(std::move(error));
        }

        auto start = elif_token.getArea();

        // consume the ( Token
        auto open_l_token_res = if_expr_lexer().peek_and_pop();
        if(not open_l_token_res.has_value()) {
            return std::unexpected(std::move(open_l_token_res.error()));
        }
        auto open_l_token = std::move(open_l_token_res.value());

        if(open_l_token.getType() != lexing::TokenTypes::L_PARANTHESIS) {
            UnexpectedToken error{open_l_token.getType(),
                                  open_l_token.getArea(),
                                  TokenTypes::L_PARANTHESIS};

            return std::unexpected(std::move(error));
        }

        auto elif_condition_res = static_cast<T*>(this)->expression();
        if(not elif_condition_res) {
            return std::unexpected(std::move(elif_condition_res.error()));
        }
        auto elif_condition = std::move(elif_condition_res.value());

        // consume the ) Token
        auto open_r_token_res = if_expr_lexer().peek_and_pop();
        if(not open_r_token_res.has_value()) {
            return std::unexpected(std::move(open_r_token_res.error()));
        }
        auto open_r_token = std::move(open_r_token_res.value());

        if(open_r_token.getType() != lexing::TokenTypes::R_PARANTHESIS) {
            UnexpectedToken error{open_r_token.getType(),
                                  open_r_token.getArea(),
                                  TokenTypes::R_PARANTHESIS};

            return std::unexpected(std::move(error));
        }

        auto block_expr_res = static_cast<T*>(this)->expression();
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
    constexpr auto if_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
