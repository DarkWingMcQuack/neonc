#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

template<class T>
class BlockExpressionParser
{
public:
    constexpr auto block_expression() noexcept -> std::optional<ast::BlockExpr>
    {
        auto result = try_singe_expr_block();
        if(std::holds_alternative<std::nullopt_t>(result)) {
            // TODO: propagate error
            return std::nullopt;
        }

        if(std::holds_alternative<ast::BlockExpr>(result)) {
            return std::move(std::get<ast::BlockExpr>(result));
        }

        auto [start, stmts] = std::move(std::get<1>(result));

        // clang-format off
                if(not(block_expr_lexer().pop_next_is(lexing::TokenTypes::SEMICOLON) or
                           block_expr_lexer().pop_next_is(lexing::TokenTypes::NEWLINE))) {
                  // TODO: return error "expected newline or ;"
                  return std::nullopt;
                }
        // clang-format on

        while(not block_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            auto stmt_opt = static_cast<T*>(this)->statement();
            if(not stmt_opt.has_value()) {
                // TODO: propagate error
                return std::nullopt;
            }
            auto stmt = std::move(stmt_opt.value());

            stmts.emplace_back(std::move(stmt));

            // clang-format off
                        if(not(block_expr_lexer().pop_next_is(lexing::TokenTypes::SEMICOLON) or
                                   block_expr_lexer().pop_next_is(lexing::TokenTypes::NEWLINE))) {
                                // TODO: return error "expected newline or ;"
                                return std::nullopt;
                        }
            // clang-format on
        }

        if(not block_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            // TODO: return error "a block expression needs to return an expression with =>expr"
            return std::nullopt;
        }

        auto expr_opt = static_cast<T*>(this)->expression();
        if(not expr_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto expr = std::move(expr_opt.value());

        if(not block_expr_lexer().next_is(lexing::TokenTypes::R_BRACKET)) {
            // TODO: return error: "expected } after block return statment"
            return std::nullopt;
        }

        auto end = block_expr_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);
        return ast::BlockExpr{area, std::move(stmts), std::move(expr)};
    }

private:
    constexpr auto try_singe_expr_block() noexcept
        -> std::variant<ast::BlockExpr,
                        std::pair<lexing::TextArea, std::vector<ast::Statement>>,
                        std::nullopt_t>
    {
        if(not block_expr_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
            return std::nullopt;
        }

        auto start = block_expr_lexer().peek_and_pop().value().getArea();

        // handle the case of a expr block which only contains a
        //{=> <expr>} without any statements pre the return statement
        if(block_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            auto ret_expr_opt = static_cast<T*>(this)->expression();
            if(not ret_expr_opt.has_value()) {
                // TODO: propagate error
                return std::nullopt;
            }
            auto ret_expr = std::move(ret_expr_opt.value());

            if(not block_expr_lexer().next_is(lexing::TokenTypes::R_BRACKET)) {
                // TODO: return "expected }" error
                return std::nullopt;
            }

            auto end = block_expr_lexer().peek_and_pop().value().getArea();
            auto area = lexing::TextArea::combine(start, end);

            return ast::BlockExpr{std::move(area),
                                  {},
                                  std::move(ret_expr)};
        }

        auto stmt_opt = static_cast<T*>(this)->statement();
        if(not stmt_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto stmt = std::move(stmt_opt.value());

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
    constexpr auto block_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
