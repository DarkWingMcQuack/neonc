#pragma once

#include <algorithm>
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class which parses all expressions starting with (
// those can be grouped expression like (1+1), tuples like (1, 2)
// or lambda expressions
template<class T>
class LParExpressionParser
{
public:
    constexpr auto l_par_expression() noexcept -> std::optional<ast::Expression>
    {
        if(not lpar_expr_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
            return std::nullopt;
        }

        auto start = lpar_expr_lexer().peek_and_pop().value().getArea();

        // parse the first expression after the ( because thats what all the three options
        // what it could be have in common
        auto expr_opt = static_cast<T*>(this)->expression();
        if(not expr_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto expr = std::move(expr_opt.value());

        // if the next token is ) then it was a grouped expression or a lambda
        if(lpar_expr_lexer().pop_next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // check if it could be a lambda otherwise return the expr itself
            return try_lambda(start, std::move(expr));
        }

        // TODO: if next token is a : then it is a lambda expression

        // if next token is a , then we have (<expr>, which coule be a lambda expression "(a, b) => c" or a tuple (a, b)
        if(lpar_expr_lexer().next_is(lexing::TokenTypes::COMMA)) {

            // try parsing a tuple or a lambda expression from the expressions lists
            auto res = tuple_or_lambda_params(start, std::move(expr));

            return std::visit(
                [this, start = start](auto&& r) -> std::optional<ast::Expression> {
                    if constexpr(std::same_as<std::nullopt_t, std::decay_t<decltype(r)>>) {
                        return std::nullopt;
                    } else {
                        // r can be a list of parameters or a tuple
                        // if it is a list of parameters, it is clear that it is a lambda expression
                        // but if it is a tuple, it can still be a lambda expression when it is followed by a =>
                        return try_lambda(start, std::move(r));
                    }
                },
                std::move(res));
        }

        // TODO: return error "expected ) or : or ,"
        return std::nullopt;
    }

private:
    // given a list of exprssions, this method checks if all the expressions are identifiers
    // and if so it converts them to a lits of lambda parameters
    constexpr auto exprs_to_parameters(std::vector<ast::Expression>&& parsed) noexcept
        -> std::optional<std::vector<ast::LambdaParameter>>
    {
        for(const auto& elem : parsed) {
            if(not std::holds_alternative<ast::Identifier>(elem)) {
                // TODO: return error "elem" should be an identifier and not an ...
                return std::nullopt;
            }
        }


        std::vector<ast::LambdaParameter> parameters;
        parameters.reserve(parsed.size());

        std::transform(std::make_move_iterator(std::begin(parsed)),
                       std::make_move_iterator(std::end(parsed)),
                       std::back_inserter(parameters),
                       [](auto&& expr) {
                           auto id = std::move(std::get<ast::Identifier>(expr));
                           return ast::LambdaParameter{std::move(id)};
                       });

        return std::move(parameters);
    }

    // parse a lambda parameter <id> (: <type>)?
    constexpr auto lambda_parameter() noexcept -> std::optional<ast::LambdaParameter>
    {
        auto id_opt = static_cast<T*>(this)->identifier();
        if(not id_opt.has_value()) {
            // TODO: propagte error
            return std::nullopt;
        }
        auto id = std::move(id_opt.value());

        if(lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COLON)) {
            auto type_opt = static_cast<T*>(this)->type();
            if(not type_opt.has_value()) {
                // TODO: propagte error
                return std::nullopt;
            }
            auto type = std::move(type_opt.value());

            return ast::LambdaParameter{std::move(id), std::move(type)};
        }
        return ast::LambdaParameter{std::move(id)};
    }

    // parse a lambda parameter which is a simple identifier or "id: <type>" given the id is already parsed
    // and the start is at the colon right before the type
    constexpr auto lambda_parameter(ast::Identifier&& id) noexcept -> std::optional<ast::LambdaParameter>
    {
        if(lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COLON)) {
            auto type_opt = static_cast<T*>(this)->type();
            if(not type_opt.has_value()) {
                // TODO: propagte error
                return std::nullopt;
            }
            auto type = std::move(type_opt.value());

            return ast::LambdaParameter{std::move(id), std::move(type)};
        }
        return ast::LambdaParameter{std::move(id)};
    }

    // when parsing a tuple and we have a list of already parsed expressions and then at a point the next expression is <expr>:<type>
    // it is known that the tuple wasnt realy a tuple but a lambda parameter list
    // this method starts right at the first colon
    // it extracts all the already parsed expressions, checks if they are identifiers and if to assignes the type after the first colon
    // to the last parsed identifier
    // then it continues to parse the list of lambda parameters
    constexpr auto lambda_parameters_from_starting_tuple(std::vector<ast::Expression>&& parsed) noexcept
        -> std::optional<std::vector<ast::LambdaParameter>>
    {
        // sanity check
        if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COLON)) {
            // TODO: log something
            return std::nullopt;
        }

        auto type_opt = static_cast<T*>(this)->type();
        if(not type_opt.has_value()) {
            // TODO: propagte error
            return std::nullopt;
        }
        auto type = std::move(type_opt.value());

        auto params_opt = exprs_to_parameters(std::move(parsed));
        if(not params_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto params = std::move(params_opt.value());

        params.back().setType(std::move(type));

        while(lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COMMA)) {
            auto param_opt = lambda_parameter();
            if(not param_opt.has_value()) {
                return std::nullopt;
            }

            params.emplace_back(std::move(param_opt.value()));
        }

        if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // TODO: return "expected ) error"
            return std::nullopt;
        }

        return std::make_optional(std::move(params));
    }

    // given (<expr> we try to parse a tuple or a lambda parameter list here
    // when in any case there is a "id : type" pattern in the parsing then it musst be a lambda parameter list
    // if it is just a lits of expressions without type annotation then it musst be a tuple
    constexpr auto tuple_or_lambda_params(lexing::TextArea start, ast::Expression&& first) noexcept
        -> std::variant<ast::TupleExpr,
                        std::vector<ast::LambdaParameter>,
                        std::nullopt_t>
    {
        std::vector<ast::Expression> expressions;
        expressions.emplace_back(std::move(first));

        while(lpar_expr_lexer().pop_next_is(lexing::TokenTypes::COMMA)) {
            auto expr_opt = static_cast<T*>(this)->expression();
            if(not expr_opt) {
                // TODO: propagate error
                return std::nullopt;
            }
            auto expr = std::move(expr_opt.value());
            expressions.emplace_back(std::move(expr));
        }

        // do not pop the colon
        if(lpar_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
            auto params_opt = lambda_parameters_from_starting_tuple(std::move(expressions));
            if(not params_opt.has_value()) {
                return std::nullopt;
            }
            return std::move(params_opt.value());
        }

        if(not lpar_expr_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // TODO: return "expected ) error"
            return std::nullopt;
        }

        auto end = lpar_expr_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);

        return ast::TupleExpr{area, std::move(expressions)};
    }


    // given a list of lambda parameters such as (a: Int, b, c: Int) it musst be a lambda expression and therefore be followed by a => token
    // consume this token, parse the body and return the lambda expression
    constexpr auto try_lambda(lexing::TextArea start, std::vector<ast::LambdaParameter>&& parameters) noexcept
        -> std::optional<ast::Expression>
    {
        if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            // TODO: return error "expected =>"
            return std::nullopt;
        }

        auto body_opt = static_cast<T*>(this)->expression();
        if(not body_opt.has_value()) {
            return std::nullopt;
        }
        auto body = std::move(body_opt.value());
        auto end = ast::getTextArea(body);
        auto area = lexing::TextArea::combine(start, end);

        return ast::Expression{
            ast::forward<ast::LambdaExpr>(area,
                                          std::move(parameters),
                                          std::move(body))};
    }

    // given a parsed tuple expression it could be the case that this is a lambda expression
    // to this be the case the tuple expression needs to be followed by a => token
    // if this is the case it is a lambda expression and all the expressions in the tuple need to be identifiers
    // if the tuple is not followed by a => token we just return the tuple itself, because it is just a tuple and not an lambda expression
    constexpr auto try_lambda(lexing::TextArea start, ast::TupleExpr&& tuple) noexcept
        -> std::optional<ast::Expression>
    {
        // if the next identifier is not a lambda arrow then the expression was realy a tuple expression
        if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            return ast::Expression{
                ast::forward<ast::TupleExpr>(std::move(tuple))};
        }

        // parse the body of the lambda expression
        auto body_opt = static_cast<T*>(this)->expression();
        if(not body_opt.has_value()) {
            return std::nullopt;
        }
        auto body = std::move(body_opt.value());

        // calculate the area of the lambda expression
        auto end = ast::getTextArea(body);
        auto area = lexing::TextArea::combine(start, end);

        // move expressions out of the tuple
        auto exprs = std::move(tuple.getExpressions());

        // check if all expressions are identifiers
        auto param_opt = exprs_to_parameters(std::move(exprs));
        if(not param_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto parameters = std::move(param_opt.value());

        return ast::Expression{
            ast::forward<ast::LambdaExpr>(area,
                                          std::move(parameters),
                                          std::move(body))};
    }
    // given an expression after a (expr)=> we try to parse a lambda here
    // for this to be a valid lambda, the expression needs to be a parameter which means it needs to be an Identifier
    // if that is true we parse the => followed by the lambda body
    // if the expr in (<expresssion>) is not followed by a => it is only a grouped expression and we return the simple expression again
    constexpr auto try_lambda(lexing::TextArea start, ast::Expression&& first) noexcept
        -> std::optional<ast::Expression>
    {
        // if the next identifier is not a lambda arrow then the expression was realy a tuple expression
        if(not lpar_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            return std::move(first);
        }

        auto body_opt = static_cast<T*>(this)->expression();
        if(not body_opt.has_value()) {
            return std::nullopt;
        }
        auto body = std::move(body_opt.value());

        auto end = ast::getTextArea(body);
        auto area = lexing::TextArea::combine(start, end);

        std::vector<ast::Expression> exprs;
        exprs.emplace_back(std::move(first));

        auto param_opt = exprs_to_parameters(std::move(exprs));
        if(not param_opt.has_value()) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto parameters = std::move(param_opt.value());

        return ast::Expression{
            ast::forward<ast::LambdaExpr>(area,
                                          std::move(parameters),
                                          std::move(body))};
    }

private:
    constexpr auto lpar_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
