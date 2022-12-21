#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
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
    constexpr auto statement() noexcept -> std::optional<ast::Statement>
    {
        if(stmt_lexer().next_is(lexing::TokenTypes::LET)) {
            return static_cast<T*>(this)->let();
        }

        if(stmt_lexer().next_is(lexing::TokenTypes::WHILE)) {
            return wile();
        }

        if(stmt_lexer().next_is(lexing::TokenTypes::IF)) {
            return ifStmt();
        }

        if(stmt_lexer().next_is(lexing::TokenTypes::FOR)) {
            return forStmt();
        }

        // TODO: just return static_cast<T*>(this)->expression();
        if(auto res = static_cast<T*>(this)->expression()) {
            return std::move(res.value());
        }

        return std::nullopt;
    }

private:
    constexpr auto stmt_list() noexcept -> std::optional<std::vector<ast::Statement>>
    {
        std::vector<ast::Statement> stmts;

        while(true) {
            auto stmt_opt = statement();
            if(not stmt_opt.has_value()) {
                // TODO: propagate error
                return std::nullopt;
            }
            stmts.emplace_back(std::move(stmt_opt.value()));

            auto next_token_opt = stmt_lexer().peek_and_pop();
            if(not next_token_opt.has_value()) {
                // TODO: return expected a token from the lexer, got none this seems to be a lexer bug
                return std::nullopt;
            }
            auto next_token = std::move(next_token_opt.value());

            if(not next_token.isSeparator()) {
                // TODO: return error next token expected to be a ; or \n
                return std::nullopt;
            }
        }

        return stmts;
    }

    constexpr auto block() noexcept -> std::optional<std::pair<std::vector<ast::Statement>, lexing::TextArea>>
    {
        if(not stmt_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
            // TODO: return expected {
            return std::nullopt;
        }
        auto start = stmt_lexer().peek_and_pop().value().getArea();

        auto body_opt = stmt_list();
        if(not body_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto body = std::move(body_opt.value());


        if(not stmt_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
            // TODO: return expected }
            return std::nullopt;
        }

        auto end = stmt_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);

        return std::pair{std::move(body), std::move(area)};
    }

    constexpr auto wile() noexcept -> std::optional<ast::Statement>
    {
        // sanity check
        if(not stmt_lexer().next_is(lexing::TokenTypes::WHILE)) {
            // TODO: return expexted "while" token
            return std::nullopt;
        }

        auto start = stmt_lexer().peek_and_pop().value().getArea();

        auto condition_opt = static_cast<T*>(this)->expression();
        if(not condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto condition = std::move(condition_opt.value());

        auto body_opt = block();
        if(not body_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto [body, end] = std::move(body_opt.value());
        auto area = lexing::TextArea::combine(start, end);

        return ast::Statement{
            ast::forward<ast::WhileStmt>(area,
                                         std::move(condition),
                                         std::move(body))};
    }

    constexpr auto forStmt() noexcept -> std::optional<ast::Statement>
    {
        // sanity check
        if(not stmt_lexer().next_is(lexing::TokenTypes::FOR)) {
            return std::nullopt;
        }
        auto start = stmt_lexer().peek_and_pop().value().getArea();

        // clang-format off
                if(not stmt_lexer().next_is(lexing::TokenTypes::L_BRACKET) and
                   not stmt_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
                        // TODO: return error "expected ( or {"
                        return std::nullopt;
                }
        // clang-format on

        auto enclosing_type = stmt_lexer().peek_and_pop().value().getType();

        std::vector<ast::ForElement> elems;

        // clang-format off
                while(not stmt_lexer().next_is(lexing::TokenTypes::L_BRACKET) and
                          not stmt_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {

                        auto elem_opt = stmt_for_element();
                        if(not elem_opt.has_value()) {
                                // TODO: propagate error
                                return std::nullopt;
                        }

                        elems.emplace_back(std::move(elem_opt.value()));
                }
        // clang-format on

        if(not stmt_lexer().pop_next_is(enclosing_type)) {
            // TODO: return "expected $enclosing_type" error
            return std::nullopt;
        }

        auto block_opt = block();
        if(not block_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto [block, end] = std::move(block_opt.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::Statement{
            ast::forward<ast::ForStmt>(std::move(area),
                                       std::move(elems),
                                       std::move(block))};
    }

    constexpr auto elseStmt() noexcept -> std::optional<ast::ElseStmt>
    {
        if(not stmt_lexer().next_is(lexing::TokenTypes::ELSE)) {
            // TODO: return error "expected else kexword"
            return std::nullopt;
        }

        auto start = stmt_lexer().peek_and_pop().value().getArea();

        auto body_opt = block();
        if(not body_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto [body, end] = std::move(body_opt.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::ElseStmt{area, std::move(body)};
    }

    constexpr auto elifStmt() noexcept -> std::optional<ast::ElifStmt>
    {
        if(not stmt_lexer().next_is(lexing::TokenTypes::ELIF)) {
            return std::nullopt;
        }

        auto start = stmt_lexer().peek_and_pop().value().getArea();

        auto elif_condition_opt = static_cast<T*>(this)->expression();
        if(not elif_condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto elif_condition = std::move(elif_condition_opt.value());

        auto body_opt = block();
        if(not body_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto [body, end] = std::move(body_opt.value());

        auto area = lexing::TextArea::combine(start, end);

        return ast::ElifStmt{area,
                             std::move(elif_condition),
                             std::move(body)};
    }

    constexpr auto ifStmt() noexcept -> std::optional<ast::Statement>
    {
        if(not stmt_lexer().next_is(lexing::TokenTypes::IF)) {
            return std::nullopt;
        }

        auto start = stmt_lexer().peek_and_pop().value().getArea();

        auto condition_opt = static_cast<T*>(this)->expression();
        if(not condition_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto condition = std::move(condition_opt.value());

        auto body_opt = block();
        if(not body_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto [body, end] = std::move(body_opt.value());

        auto area = lexing::TextArea::combine(start, end);

        std::vector<ast::ElifStmt> elifs;
        while(stmt_lexer().next_is(lexing::TokenTypes::ELIF)) {
            auto elif_opt = elifStmt();
            if(not elif_opt) {
                // TODO: propagate error
                return std::nullopt;
            }
            elifs.emplace_back(std::move(elif_opt.value()));
            area = lexing::TextArea::combine(start, elifs.back().getArea());
        }

        std::optional<ast::ElseStmt> else_;
        if(stmt_lexer().next_is(lexing::TokenTypes::ELSE)) {
            auto else_opt = elseStmt();
            if(not else_opt.has_value()) {
                // TODO: propagate error
                return std::nullopt;
            }
            area = lexing::TextArea::combine(start, else_opt.value().getArea());
            else_ = std::move(else_opt.value());
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
        -> std::optional<ast::Identifier>
    {
        return static_cast<T*>(this)->identifier();
    }

    constexpr auto stmt_for_element() noexcept -> std::optional<ast::ForElement>
    {
        return static_cast<T*>(this)->for_element();
    }

    constexpr auto stmt_type() noexcept -> std::optional<ast::Type>
    {
        return static_cast<T*>(this)->type();
    }

    constexpr auto stmt_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
