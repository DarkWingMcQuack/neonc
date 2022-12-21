#pragma once

#include "common/Error.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <optional>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for statements,
// 1. while loop
// 2. let statment
// 3. any expression
// 8. if-else statement
template<class T>
class StatmentParser
{
public:
    constexpr auto statement() noexcept
        -> std::expected<ast::Statement, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        auto token_res = stmt_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() == TokenTypes::LET) {
            return static_cast<T*>(this)->let();
        }

        if(token.getType() == TokenTypes::WHILE) {
            return wile();
        }

        if(token.getType() == TokenTypes::IF) {
            return ifStmt();
        }

        if(token.getType() == TokenTypes::FOR) {
            return forStmt();
        }

        if(auto res = static_cast<T*>(this)->expression()) {
            return std::move(res.value());
        }

        UnexpectedToken error{
            token.getType(),
            token.getArea(),
            TokenTypes::WHILE,
            TokenTypes::LET,
            TokenTypes::FOR,
            TokenTypes::IF,
            TokenTypes::IDENTIFIER,
            TokenTypes::DOUBLE,
            TokenTypes::INTEGER,
            TokenTypes::STANDARD_STRING,
            TokenTypes::TRUE,
            TokenTypes::FALSE,
            TokenTypes::SELF_VALUE,
            TokenTypes::PLUS,
            TokenTypes::MINUS,
            TokenTypes::LOGICAL_NOT,
            TokenTypes::L_BRACKET,
            TokenTypes::L_PARANTHESIS};


        return std::unexpected(std::move(error));
    }

private:
    constexpr auto stmt_list() noexcept
        -> std::expected<std::vector<ast::Statement>, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        std::vector<ast::Statement> stmts;

        while(true) {
            auto stmt_res = statement();
            if(not stmt_res.has_value()) {
                return std::unexpected(std::move(stmt_res.error()));
            }
            stmts.emplace_back(std::move(stmt_res.value()));

            auto next_token_res = stmt_lexer().peek_and_pop();
            if(not next_token_res.has_value()) {
                return std::unexpected(std::move(next_token_res.error()));
            }
            auto next_token = std::move(next_token_res.value());

            if(not next_token.isSeparator()) {
                UnexpectedToken error{
                    next_token.getType(),
                    next_token.getArea(),
                    TokenTypes::NEWLINE,
                    TokenTypes::SEMICOLON};

                return std::unexpected(std::move(error));
            }
        }

        return stmts;
    }

    constexpr auto block() noexcept
        -> std::expected<std::pair<std::vector<ast::Statement>, lexing::TextArea>,
                         common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::L_BRACKET) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::L_BRACKET};

            return std::unexpected(std::move(error));
        }

        auto body_res = stmt_list();
        if(not body_res) {
            return std::unexpected(std::move(body_res.error()));
        }
        auto body = std::move(body_res.value());


        auto end_token_res = stmt_lexer().peek_and_pop();
        if(not end_token_res.has_value()) {
            return std::unexpected(std::move(end_token_res.error()));
        }
        auto end_token = std::move(end_token_res.value());
        auto end = end_token.getArea();

        if(end_token.getType() != TokenTypes::R_BRACKET) {
            UnexpectedToken error{
                end_token.getType(),
                end_token.getArea(),
                TokenTypes::R_BRACKET};

            return std::unexpected(std::move(error));
        }

        auto area = lexing::TextArea::combine(start, end);

        return std::pair{std::move(body), std::move(area)};
    }

    constexpr auto wile() noexcept
        -> std::expected<ast::Statement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::WHILE) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::WHILE};

            return std::unexpected(std::move(error));
        }

        auto condition_res = static_cast<T*>(this)->expression();
        if(not condition_res.has_value()) {
            return std::unexpected(std::move(condition_res.error()));
        }
        auto condition = std::move(condition_res.value());

        auto body_res = block();
        if(not body_res.has_value()) {
            return std::unexpected(std::move(body_res.error()));
        }
        auto [body, end] = std::move(body_res.value());
        auto area = lexing::TextArea::combine(start, end);

        return ast::Statement{
            ast::forward<ast::WhileStmt>(area,
                                         std::move(condition),
                                         std::move(body))};
    }

    constexpr auto forStmt() noexcept
        -> std::expected<ast::Statement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::FOR) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::FOR};

            return std::unexpected(std::move(error));
        }

        auto open_token_res = stmt_lexer().peek_and_pop();
        if(not open_token_res.has_value()) {
            return std::unexpected(std::move(open_token_res.error()));
        }
        auto open_token = std::move(open_token_res.value());
        auto open_type = open_token.getType();

        // clang-format off
        if(open_type != TokenTypes::L_PARANTHESIS and
           open_type != TokenTypes::L_BRACKET) {
            UnexpectedToken error{
                open_token.getType(),
                open_token.getArea(),
                TokenTypes::L_PARANTHESIS,
                TokenTypes::L_BRACKET};

            return std::unexpected(std::move(error));
        }
        // clang-format on


        std::vector<ast::ForElement> elems;

        // clang-format off
        while(not stmt_lexer().next_is(lexing::TokenTypes::R_BRACKET) and
              not stmt_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {

            auto elem_res = stmt_for_element();
            if(not elem_res.has_value()) {
                return std::unexpected(std::move(elem_res.error()));
            }

            elems.emplace_back(std::move(elem_res.value()));
        }
        // clang-format on

        auto close_token_res = stmt_lexer().peek_and_pop();
        if(not close_token_res.has_value()) {
            return std::unexpected(std::move(close_token_res.error()));
        }
        auto close_token = std::move(close_token_res.value());

        auto expected_close_type = open_type == TokenTypes::L_BRACKET
            ? TokenTypes::R_BRACKET
            : TokenTypes::R_PARANTHESIS;


        if(close_token.getType() != expected_close_type) {
            UnexpectedToken error{
                close_token.getType(),
                close_token.getArea(),
                expected_close_type};

            return std::unexpected(std::move(error));
        }

        auto block_res = block();
        if(not block_res.has_value()) {
            return std::unexpected(std::move(block_res.error()));
        }
        auto [block, end] = std::move(block_res.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::Statement{
            ast::forward<ast::ForStmt>(std::move(area),
                                       std::move(elems),
                                       std::move(block))};
    }

    constexpr auto elseStmt() noexcept
        -> std::expected<ast::ElseStmt, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::ELSE) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::ELSE};

            return std::unexpected(std::move(error));
        }

        auto block_res = block();
        if(not block_res.has_value()) {
            return std::unexpected(std::move(block_res.error()));
        }
        auto [body, end] = std::move(block_res.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::ElseStmt{area, std::move(body)};
    }

    constexpr auto elifStmt() noexcept
        -> std::expected<ast::ElifStmt, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::ELIF) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::ELIF};

            return std::unexpected(std::move(error));
        }

        auto elif_condition_res = static_cast<T*>(this)->expression();
        if(not elif_condition_res) {
            return std::unexpected(std::move(elif_condition_res.error()));
        }
        auto elif_condition = std::move(elif_condition_res.value());

        auto block_res = block();
        if(not block_res.has_value()) {
            return std::unexpected(std::move(block_res.error()));
        }
        auto [body, end] = std::move(block_res.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::ElifStmt{area,
                             std::move(elif_condition),
                             std::move(body)};
    }

    constexpr auto ifStmt() noexcept
        -> std::expected<ast::Statement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_token_res = stmt_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start_token = std::move(start_token_res.value());
        auto start = start_token.getArea();

        if(start_token.getType() != TokenTypes::ELIF) {
            UnexpectedToken error{
                start_token.getType(),
                start_token.getArea(),
                TokenTypes::ELIF};

            return std::unexpected(std::move(error));
        }

        auto condition_res = static_cast<T*>(this)->expression();
        if(not condition_res) {
            return std::unexpected(std::move(condition_res.error()));
        }
        auto condition = std::move(condition_res.value());

        auto block_res = block();
        if(not block_res.has_value()) {
            return std::unexpected(std::move(block_res.error()));
        }
        auto [body, end] = std::move(block_res.value());

        auto area = lexing::TextArea::combine(start, end);

        std::vector<ast::ElifStmt> elifs;
        while(stmt_lexer().next_is(lexing::TokenTypes::ELIF)) {
            auto elif_res = elifStmt();
            if(not elif_res) {
                return std::unexpected(std::move(elif_res.error()));
            }
            elifs.emplace_back(std::move(elif_res.value()));
            area = lexing::TextArea::combine(start, elifs.back().getArea());
        }

        std::optional<ast::ElseStmt> else_;
        if(stmt_lexer().next_is(lexing::TokenTypes::ELSE)) {
            auto else_res = elseStmt();
            if(not else_res.has_value()) {
                return std::unexpected(std::move(else_res.error()));
            }
            area = lexing::TextArea::combine(start, else_res.value().getArea());
            else_ = std::move(else_res.value());
        }

        return ast::Statement{
            ast::forward<ast::IfStmt>(area,
                                      std::move(condition),
                                      std::move(body),
                                      std::move(elifs),
                                      std::move(else_))};
    }



private:
    constexpr auto stmt_identifier() noexcept
        -> std::expected<ast::Identifier, common::error::Error>
    {
        return static_cast<T*>(this)->identifier();
    }

    constexpr auto stmt_for_element() noexcept -> std::expected<ast::ForElement, common::error::Error>
    {
        return static_cast<T*>(this)->for_element();
    }

    constexpr auto stmt_type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        return static_cast<T*>(this)->type();
    }

    constexpr auto stmt_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
