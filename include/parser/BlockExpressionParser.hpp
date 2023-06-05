#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <ast/expression/BlockExpr.hpp>
#include <common/Error.hpp>
#include <functional>
#include <lexer/Lexer.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

template<class T>
class BlockExpressionParser
{
public:
    constexpr auto block_expression() noexcept -> std::expected<ast::BlockExpr, common::error::Error>
    {
        using lexing::TokenTypes;
        using lexing::TextArea;

        auto result_res = try_singe_expr_block();
        if(not result_res.has_value()) {
            return std::unexpected(std::move(result_res.error()));
        }
        auto result = std::move(result_res.value());

        if(std::holds_alternative<ast::BlockExpr>(result)) {
            return std::move(std::get<ast::BlockExpr>(result));
        }

        auto [start, stmts] = std::move(std::get<1>(result));

        if(auto res = expect<TokenTypes::SEMICOLON, TokenTypes::NEWLINE>();
           not res.has_value()) {
            return std::unexpected(std::move(res.error()));
        }


        while(not block_expr_lexer().next_is(TokenTypes::LAMBDA_ARROW)) {

            auto stmt_res = static_cast<T *>(this)->statement();
            if(not stmt_res.has_value()) {
                return std::unexpected(std::move(stmt_res.error()));
            }

            stmts.emplace_back(std::move(stmt_res.value()));

            if(auto res = expect<TokenTypes::SEMICOLON, TokenTypes::NEWLINE>();
               not res.has_value()) {
                return std::unexpected(std::move(res.error()));
            }
        }

        if(auto res = expect<TokenTypes::LAMBDA_ARROW>();
           not res.has_value()) {
            return std::unexpected(std::move(res.error()));
        }

        auto expr_res = static_cast<T *>(this)->expression();
        if(not expr_res.has_value()) {
            return std::unexpected(std::move(expr_res.error()));
        }
        auto expr = std::move(expr_res.value());

        auto end_res = expect<TokenTypes::R_BRACKET>();
        if(not end_res.has_value()) {
            return std::unexpected(std::move(end_res.error()));
        }
        auto end = end_res.value().getArea();

        auto area = TextArea::combine(start, end);
        return ast::BlockExpr{area, std::move(stmts), std::move(expr)};
    }

private:
    constexpr auto try_singe_expr_block() noexcept
        -> std::expected<
            std::variant<ast::BlockExpr,
                         std::pair<lexing::TextArea,
                                   std::vector<ast::Statement>>>,
            common::error::Error>
    {
        using lexing::TextArea;
        using common::error::Error;
        using lexing::TokenTypes;

        auto start_res = expect<TokenTypes::L_BRACKET>();
        if(not start_res.has_value()) {
            return std::unexpected(std::move(start_res.error()));
        }
        auto start = start_res.value().getArea();

        // handle the case of a expr block which only contains a
        //{=> <expr>} without any statements pre the return statement
        if(block_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            auto ret_expr_res = static_cast<T *>(this)->expression();
            if(not ret_expr_res.has_value()) {
                return std::unexpected(std::move(ret_expr_res.error()));
            }
            auto ret_expr = std::move(ret_expr_res.value());

            auto end_res = expect<TokenTypes::R_BRACKET>();
            if(not end_res.has_value()) {
                return std::unexpected(std::move(end_res.error()));
            }
            auto end = end_res.value().getArea();
            auto area = TextArea::combine(start, end);

            return ast::BlockExpr{std::move(area),
                                  {},
                                  std::move(ret_expr)};
        }

        auto stmt_res = static_cast<T *>(this)->statement();
        if(not stmt_res.has_value()) {
            return std::unexpected(std::move(stmt_res.error()));
        }
        auto stmt = std::move(stmt_res.value());

        if(block_expr_lexer().next_is(lexing::TokenTypes::R_BRACKET)
           and std::holds_alternative<ast::Expression>(stmt)) {

            auto end = block_expr_lexer().peek_and_pop().value().getArea();
            auto area = lexing::TextArea::combine(start, end);
            auto ret_expr = std::move(std::get<ast::Expression>(stmt));
            return ast::BlockExpr{std::move(area),
                                  {},
                                  std::move(ret_expr)};
        }

        std::vector<ast::Statement> stmts;
        stmts.emplace_back(std::move(stmt));

        return std::pair{start, std::move(stmts)};
    }


private:
    template<lexing::TokenTypes... types>
    constexpr auto expect() noexcept -> std::expected<lexing::Token, common::error::Error>
    {
        return block_expr_lexer().template expect<types...>();
    }

    constexpr auto block_expr_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};

} // namespace parser
