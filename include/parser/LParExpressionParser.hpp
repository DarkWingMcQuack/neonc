#pragma once

#include <algorithm>
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <ast/common/Identifier.hpp>
#include <ast/expression/LambdaExpr.hpp>
#include <ast/expression/TupleExpr.hpp>
#include <common/Error.hpp>
#include <exception>
#include <expected>
#include <fmt/core.h>
#include <iterator>
#include <lexer/Lexer.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <optional>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>
#include <variant>

namespace parser {

// crtp class which parses all expressions starting with (
// those can be grouped expression like (1+1), tuples like (1, 2)
// or lambda expressions
template<class T>
class LParExpressionParser
{
public:
    constexpr auto l_par_expression() noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        auto start_token_res = lpar_expr_lexer().peek_and_pop();
        if(not start_token_res.has_value()) {
            return std::unexpected(std::move(start_token_res.error()));
        }
        auto start = start_token_res.value().getArea();

        std::vector<std::variant<ast::Identifier, ast::LambdaParameter, ast::Expression>> results;

        bool is_lambda = false;
        bool is_tuple = false;

        while(true) {
            auto next_elem_res = parse_next_element();
            if(not next_elem_res.has_value()) {
                return std::unexpected(std::move(next_elem_res.error()));
            }
            auto next_elem = std::move(next_elem_res.value());


            is_tuple |= std::holds_alternative<ast::Expression>(next_elem);
            is_lambda |= std::holds_alternative<ast::LambdaParameter>(next_elem);

            results.emplace_back(std::move(next_elem));

            if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COMMA)) {
                break;
            }
        }

        auto r_par_token_res = lpar_expr_lexer().peek_and_pop();
        if(not r_par_token_res.has_value()) {
            return std::unexpected(std::move(r_par_token_res.error()));
        }
        auto r_par_token = std::move(r_par_token_res.value());
        if(r_par_token.getType() != lexing::TokenTypes::R_PARANTHESIS) {
            common::error::UnexpectedToken error{
                r_par_token.getType(),
                r_par_token.getArea(),
                lexing::TokenTypes::R_PARANTHESIS};

            return std::unexpected(std::move(error));
        }
        auto end = r_par_token.getArea();

        if(is_tuple) {
            return build_tuple(std::move(results), start, end);
        }

        if(is_lambda) {
            return build_lambda(std::move(results), start);
        }

        return lambda_or_tuple(std::move(results), start, end);
    }

private:
    // given a list of variants which are only allowed to hold expressions and identifiers
    // this function builds a tuple expression
    constexpr auto build_tuple(std::vector<std::variant<ast::Identifier, ast::LambdaParameter, ast::Expression>>&& results,
                               lexing::TextArea start,
                               lexing::TextArea end) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        std::vector<ast::Expression> exprs;
        exprs.reserve(results.size());

        std::transform(std::make_move_iterator(std::begin(results)),
                       std::make_move_iterator(std::end(results)),
                       std::back_inserter(exprs),
                       [](auto&& v) {
                           if(std::holds_alternative<ast::Identifier>(v)) {
                               auto id = std::move(std::get<ast::Identifier>(std::move(v)));
                               return ast::Expression{std::move(id)};
                           }
                           return std::move(std::get<ast::Expression>(std::move(v)));
                       });

        auto area = lexing::TextArea::combine(start, end);

        // a tuple of size 1 is not a tuple but just the expression which was paired
        if(exprs.size() == 1) {
            return std::move(exprs[0]);
        }

        return ast::Expression{ast::forward<ast::TupleExpr>(area, std::move(exprs))};
    }

    // given a list of variants which are only allowed to hold lambda parameters and identifiers
    // this function builds the lambda parameters, then parses the rest of the lambda expression and builds it
    constexpr auto build_lambda(std::vector<std::variant<ast::Identifier, ast::LambdaParameter, ast::Expression>>&& results,
                                lexing::TextArea start) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        std::vector<ast::LambdaParameter> params;
        params.reserve(results.size());

        std::transform(std::make_move_iterator(std::begin(results)),
                       std::make_move_iterator(std::end(results)),
                       std::back_inserter(params),
                       [](auto&& v) {
                           if(std::holds_alternative<ast::LambdaParameter>(v)) {
                               return std::move(std::get<ast::LambdaParameter>(std::move(v)));
                           }

                           auto id = std::move(std::get<ast::Identifier>(std::move(v)));
                           return ast::LambdaParameter{std::move(id)};
                       });


        auto arrow_token_res = lpar_expr_lexer().peek_and_pop();
        if(not arrow_token_res.has_value()) {
            return std::unexpected(std::move(arrow_token_res.error()));
        }
        auto arrow_token = std::move(arrow_token_res.value());
        if(arrow_token.getType() != lexing::TokenTypes::LAMBDA_ARROW) {
            common::error::UnexpectedToken error{
                arrow_token.getType(),
                arrow_token.getArea(),
                lexing::TokenTypes::LAMBDA_ARROW};

            return std::unexpected(std::move(error));
        }

        auto body_res = static_cast<T*>(this)->expression();
        if(not body_res.has_value()) {
            return std::unexpected(std::move(body_res.error()));
        }
        auto body = std::move(body_res.value());

        auto end = ast::getTextArea(body);
        auto area = lexing::TextArea::combine(start, end);

        return ast::Expression{
            ast::forward<ast::LambdaExpr>(area,
                                          std::move(params),
                                          std::move(body))};
    }

    // given a vector of just identifiers, this function looks ahead one token
    // and decides if it should build a tuple expression or a lambda expression
    // and then returns the right one
    constexpr auto lambda_or_tuple(std::vector<std::variant<ast::Identifier, ast::LambdaParameter, ast::Expression>>&& results,
                                   lexing::TextArea start,
                                   lexing::TextArea end) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {

        auto arrow_token_res = lpar_expr_lexer().peek();
        if(not arrow_token_res.has_value()) {
            return std::unexpected(std::move(arrow_token_res.error()));
        }
        auto arrow_token = std::move(arrow_token_res.value());

        if(arrow_token.getType() == lexing::TokenTypes::LAMBDA_ARROW) {
            return build_lambda(std::move(results), start);
        }

        return build_tuple(std::move(results), start, end);
    }

    // parses an element of an l_par expression
    constexpr auto parse_next_element() noexcept
        -> std::expected<std::variant<ast::Identifier, ast::LambdaParameter, ast::Expression>, common::error::Error>
    {
        auto peek_res = lpar_expr_lexer().template peek<2>();
        if(not peek_res.has_value()) {
            return std::unexpected(std::move(peek_res.error()));
        }
        auto [first, second] = std::move(peek_res.value());

        if(first.getType() == lexing::TokenTypes::IDENTIFIER
           and second.getType() == lexing::TokenTypes::COLON) {
            return typed_lambda_parameter();
        }

        if(first.getType() == lexing::TokenTypes::IDENTIFIER
           and second.getType() == lexing::TokenTypes::COMMA) {
            return static_cast<T*>(this)->identifier();
        }

        if(first.getType() == lexing::TokenTypes::IDENTIFIER
           and second.getType() == lexing::TokenTypes::R_PARANTHESIS) {
            return static_cast<T*>(this)->identifier();
        }

        return static_cast<T*>(this)->expression();
    }

    // parse a lambda parameter <id> : <type>
    constexpr auto typed_lambda_parameter() noexcept
        -> std::expected<ast::LambdaParameter, common::error::Error>
    {
        auto id_res = static_cast<T*>(this)->identifier();
        if(not id_res.has_value()) {
            return std::unexpected(std::move(id_res.error()));
        }
        auto id = std::move(id_res.value());

        auto colon_token_res = lpar_expr_lexer().peek_and_pop();
        if(not colon_token_res.has_value()) {
            return std::unexpected(std::move(colon_token_res.error()));
        }
        auto colon_token = std::move(colon_token_res.value());

        if(colon_token.getType() != lexing::TokenTypes::COLON) {
            common::error::UnexpectedToken error{colon_token.getType(),
                                                 colon_token.getArea(),
                                                 lexing::TokenTypes::COLON};
            return std::unexpected(std::move(error));
        }

        auto type_res = static_cast<T*>(this)->type();
        if(not type_res.has_value()) {
            return std::unexpected(std::move(type_res.error()));
        }
        auto type = std::move(type_res.value());

        return ast::LambdaParameter{std::move(id), std::move(type)};
    }

    constexpr auto lpar_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
