#pragma once

#include "common/Error.hpp"
#include "lexer/TextArea.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for expressions which are an identifier
// or an simple lambda
// this means it can parse simple identifiers
// or irdentifiers followed by a => and another expression
template<class T>
class SimpleLambdaOrIdentifierParser
{
public:
    constexpr auto identifier_or_simple_lambda() noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        auto id_res = static_cast<T*>(this)->identifier();

        if(not id_res.has_value()) {
            return std::unexpected(std::move(id_res.error()));
        }

        return simple_lambda(std::move(id_res.value()));
    }

private:
    // given an already parsed identifier this method checks if it is a lambda expression
    // if the identifier is followed by a => token it is a lambda expression and it parses the body and returns
    // a lamdba exprssion
    // if this is not the case it simply returns the already parsed parameter again
    // if the identifier is followed by a : it is assumed that this shoud be a lambda expression and after
    // the : comes the type annotation but a : b => c is not clear since it could be (a : b) => c or a: (b => c)
    // therefore this is forbitten and an error should be returned that lambda parameters with type annotations
    // need to be enclosed by ()
    constexpr auto simple_lambda(ast::Identifier&& parameter) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::LambdaAnotationsInParameterNotEnclosed;

        auto token_res = simple_lambda_or_id_expr_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() == TokenTypes::COLON) {
            auto error_area = lexing::TextArea::combine(parameter.getArea(), token.getArea());
            LambdaAnotationsInParameterNotEnclosed error{std::move(error_area)};
            return std::unexpected(std::move(error));
        }

        if(token.getType() != TokenTypes::LAMBDA_ARROW) {
            return std::move(parameter);
        }

        simple_lambda_or_id_expr_lexer().pop();


        auto expr_res = static_cast<T*>(this)->expression();
        if(not expr_res) {
            return expr_res;
        }
        auto expr = std::move(expr_res.value());

        auto start = parameter.getArea();
        auto end = ast::getTextArea(expr);
        auto area = lexing::TextArea::combine(start, end);

        std::vector<ast::LambdaParameter> parameters;
        parameters.emplace_back(std::move(parameter));

        return ast::Expression{
            ast::forward<ast::LambdaExpr>(area,
                                          std::move(parameters),
                                          std::move(expr))};
    }

private:
    constexpr auto simple_lambda_or_id_expr_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
