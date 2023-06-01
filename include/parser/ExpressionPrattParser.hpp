#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <common/Error.hpp>
#include <exception>
#include <expected>
#include <lexer/Lexer.hpp>
#include <lexer/Tokens.hpp>
#include <optional>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for parsing if expressions
template<class T>
class ExpressionPrattParser
{
public:
    constexpr auto expression() noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        return expression_bp(0);
    }

private:
    constexpr auto function_params(lexing::TokenTypes end_token) noexcept
        -> std::expected<std::vector<ast::Expression>, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        std::vector<ast::Expression> params;
        while(not expr_lexer().next_is(end_token)) {
            auto res = expression();
            if(not res.has_value()) {
                return std::unexpected(std::move(res.error()));
            }
            params.emplace_back(std::move(res.value()));

            if(expr_lexer().next_is(end_token)) {
                break;
            }

            auto token_res = expr_lexer().peek_and_pop();
            if(not token_res.has_value()) {
                return std::unexpected(std::move(token_res.error()));
            }
            auto token = std::move(token_res.value());
            if(token.getType() != TokenTypes::COMMA) {
                UnexpectedToken error{token.getType(),
                                      token.getArea(),
                                      TokenTypes::COMMA};

                return std::unexpected(std::move(error));
            }
        }

        return params;
    }

    constexpr auto expression_bp(std::uint32_t min_bp) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using common::error::Error;
        using common::error::InternalCompilerError;
        using lexing::TokenTypes;

        // lhs() parses prefix operators
        auto lhs_res = lhs();
        if(not lhs_res.has_value()) {
            return lhs_res;
        }
        auto lhs = std::move(lhs_res.value());

        while(expr_lexer().next_is_operator()) {
            const auto op = expr_lexer().peek().value();

            // parse postfix operators such as function calls
            if(auto bp_opt = postfix_binding_power(op.getType())) {
                auto bp = std::move(bp_opt.value());

                if(bp < min_bp) {
                    break;
                }

                expr_lexer().pop();

                auto end_token = op.getType() == TokenTypes::L_BRACKET
                    ? TokenTypes::R_BRACKET
                    : TokenTypes::R_PARANTHESIS;

                auto params_res = function_params(end_token);
                if(not params_res.has_value()) {
                    return std::unexpected(std::move(params_res.error()));
                }
                auto params = std::move(params_res.value());

                auto end_token_res = expr_lexer().peek_and_pop();
                if(not end_token_res.has_value()) {
                    return std::unexpected(std::move(end_token_res.error()));
                }
                auto actual_end_token = std::move(end_token_res.value());

                if(actual_end_token.getType() != end_token) {
                    UnexpectedToken error{actual_end_token.getType(),
                                          actual_end_token.getArea(),
                                          end_token};

                    return std::unexpected(std::move(error));
                }

                auto end = actual_end_token.getArea();

                lhs = build_expr(std::move(lhs), std::move(params), std::move(end));
                continue;
            }

            // parse infix operators
            auto bp_opt = infix_binding_power(op.getType());
            if(not bp_opt.has_value()) {
                Error error = InternalCompilerError{};
                return std::unexpected(std::move(error));
            }
            const auto [l_bp, r_bp] = std::move(bp_opt.value());

            if(l_bp < min_bp) {
                break;
            }

            expr_lexer().pop();

            auto rhs_res = expression_bp(r_bp);
            if(not rhs_res.has_value()) {
                return rhs_res;
            }
            auto rhs = std::move(rhs_res.value());

            auto lhs_opt = build_expr(std::move(lhs), op, std::move(rhs));
            if(not lhs_opt.has_value()) {
                return std::unexpected(InternalCompilerError{});
            }
            lhs = std::move(lhs_opt.value());
        }

        return std::move(lhs);
    }

    constexpr auto lhs() noexcept -> std::expected<ast::Expression, common::error::Error>
    {
        using common::error::Error;
        using common::error::InternalCompilerError;

        if(expr_lexer().next_is_prefix_operator()) {
            auto op = expr_lexer().peek_and_pop().value();


            auto r_bp_opt = prefix_binding_power(op.getType());
            if(not r_bp_opt.has_value()) {
                Error error = InternalCompilerError{};
                return std::unexpected(std::move(error));
            }
            auto r_bp = r_bp_opt.value();


            auto rhs_res = expression_bp(r_bp);
            if(not rhs_res.has_value()) {
                return rhs_res;
            }
            auto rhs = std::move(rhs_res.value());


            auto expr_opt = build_expr(op, std::move(rhs));
            if(not expr_opt.has_value()) {
                Error error = InternalCompilerError{};
                return std::unexpected(std::move(error));
            }

            return std::move(expr_opt.value());
        }

        return simple_expr();
    }

    constexpr auto build_expr(ast::Expression&& lhs, lexing::Token op, ast::Expression&& rhs) noexcept
        -> std::optional<ast::Expression>
    {
        auto area = lexing::TextArea::combine(ast::getTextArea(lhs),
                                              ast::getTextArea(rhs));

        switch(op.getType()) {
        case lexing::TokenTypes::PLUS:
            return ast::Expression{ast::forward<ast::Addition>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::MINUS:
            return ast::Expression{ast::forward<ast::Substraction>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::ASTERIX:
            return ast::Expression{ast::forward<ast::Multiplication>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::DIVISION:
            return ast::Expression{ast::forward<ast::Division>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::PERCENT:
            return ast::Expression{ast::forward<ast::Remainder>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::LOGICAL_OR:
            return ast::Expression{ast::forward<ast::LogicalOr>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::LOGICAL_AND:
            return ast::Expression{ast::forward<ast::LogicalAnd>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::BITWISE_OR:
            return ast::Expression{ast::forward<ast::BitwiseOr>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::BITWISE_AND:
            return ast::Expression{ast::forward<ast::BitwiseAnd>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::LT:
            return ast::Expression{ast::forward<ast::LessThen>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::LE:
            return ast::Expression{ast::forward<ast::LessEqThen>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::GT:
            return ast::Expression{ast::forward<ast::GreaterThen>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::GE:
            return ast::Expression{ast::forward<ast::GreaterEqThen>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::EQ:
            return ast::Expression{ast::forward<ast::Equal>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::NEQ:
            return ast::Expression{ast::forward<ast::NotEqual>(area, std::move(lhs), std::move(rhs))};
        case lexing::TokenTypes::DOT:
            return ast::Expression{ast::forward<ast::MemberAccess>(area, std::move(lhs), std::move(rhs))};
        default:
            return std::nullopt;
        }
    }

    constexpr auto build_expr(lexing::Token op, ast::Expression&& rhs) noexcept
        -> std::optional<ast::Expression>
    {
        auto area = lexing::TextArea::combine(op.getArea(),
                                              ast::getTextArea(rhs));

        switch(op.getType()) {
        case lexing::TokenTypes::PLUS:
            return ast::Expression{ast::forward<ast::UnaryPlus>(area, std::move(rhs))};
        case lexing::TokenTypes::MINUS:
            return ast::Expression{ast::forward<ast::UnaryMinus>(area, std::move(rhs))};
        case lexing::TokenTypes::LOGICAL_NOT:
            return ast::Expression{ast::forward<ast::LogicalNot>(area, std::move(rhs))};
        default:
            return std::nullopt;
        }
    }

    constexpr auto build_expr(ast::Expression&& lhs,
                              std::vector<ast::Expression>&& params,
                              lexing::TextArea end) noexcept
        -> ast::Expression
    {
        auto area = lexing::TextArea::combine(ast::getTextArea(lhs),
                                              end);
        return ast::Expression{
            ast::forward<ast::FunctionCall>(area,
                                            std::move(lhs),
                                            std::move(params))};
    }

    constexpr static auto infix_binding_power(lexing::TokenTypes t) noexcept
        -> std::optional<std::pair<std::uint32_t, std::uint32_t>>
    {
        switch(t) {
        case lexing::TokenTypes::LOGICAL_OR:
            return std::pair{1, 2};

        case lexing::TokenTypes::LOGICAL_AND:
            return std::pair{3, 4};

        case lexing::TokenTypes::BITWISE_OR:
            return std::pair{5, 6};

        case lexing::TokenTypes::BITWISE_AND:
            return std::pair{7, 8};

        case lexing::TokenTypes::EQ:
        case lexing::TokenTypes::NEQ:
            return std::pair{9, 10};

        case lexing::TokenTypes::LT:
        case lexing::TokenTypes::LE:
        case lexing::TokenTypes::GT:
        case lexing::TokenTypes::GE:
            return std::pair{11, 12};

        case lexing::TokenTypes::MINUS:
        case lexing::TokenTypes::PLUS:
            return std::pair{13, 14};

        case lexing::TokenTypes::ASTERIX:
        case lexing::TokenTypes::DIVISION:
        case lexing::TokenTypes::PERCENT:
            return std::pair{15, 16};

        case lexing::TokenTypes::DOT:
            return std::pair{18, 19};

        default:
            return std::nullopt;
        }
    }

    constexpr static auto prefix_binding_power(lexing::TokenTypes t) noexcept
        -> std::optional<std::uint32_t>
    {
        switch(t) {
        case lexing::TokenTypes::PLUS:
        case lexing::TokenTypes::MINUS:
        case lexing::TokenTypes::LOGICAL_NOT:
            return 17;
        default:
            return std::nullopt;
        }
    }

    constexpr static auto postfix_binding_power(lexing::TokenTypes t) noexcept
        -> std::optional<std::uint32_t>
    {
        switch(t) {
        case lexing::TokenTypes::L_BRACKET:
        case lexing::TokenTypes::L_PARANTHESIS:
            return 18;
        default:
            return std::nullopt;
        }
    }

private:
    constexpr auto expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }

    constexpr auto simple_expr() noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        return static_cast<T*>(this)->simple_expression();
    }
};
} // namespace parser
