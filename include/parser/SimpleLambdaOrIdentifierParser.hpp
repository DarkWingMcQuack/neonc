#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <common/Error.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
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
    // simple_lambda function takes a pre-parsed identifier and checks if it is a part of a lambda expression.
    // The function follows these steps:
    // - If the identifier is followed by a "=>" token, it is considered to be part of a lambda expression.
    //   The function then proceeds to parse the body of the lambda expression and returns it.
    // - If the identifier is not followed by a "=>" token, it is considered to be a standalone identifier.
    //   In this case, the function simply returns the identifier as it was received.
    // - If the identifier is followed by a ":", it is presumed to be a lambda expression with a type annotation.
    //   However, due to potential ambiguity in parsing (for example, "a : b => c" could be interpreted as
    //   "(a : b) => c" or "a : (b => c)"), this is prohibited.
    //   An error is returned in this case, advising that lambda parameters with type annotations need to be
    //   enclosed in parentheses.
    constexpr auto simple_lambda(ast::Identifier&& parameter) noexcept
        -> std::expected<ast::Expression, common::error::Error>
    {
        // Using namespaces for TokenTypes and LambdaAnotationsInParameterNotEnclosed for easier access to the types.
        using lexing::TokenTypes;
        using common::error::LambdaAnotationsInParameterNotEnclosed;

        // Attempt to get the next token from the lexer.
        auto token_res = simple_lambda_or_id_expr_lexer().peek();

        // If no token is available (error or end of input), return an error.
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }

        // If a token was successfully fetched, proceed with checking the type of the token.
        auto token = std::move(token_res.value());

        // If the token is a colon, return an error stating that lambda parameters with type annotations
        // need to be enclosed in parentheses.
        if(token.getType() == TokenTypes::COLON) {
            auto error_area = lexing::TextArea::combine(parameter.getArea(), token.getArea());
            LambdaAnotationsInParameterNotEnclosed error{std::move(error_area)};
            return std::unexpected(std::move(error));
        }

        // If the token is not a lambda arrow, return the identifier as it is.
        if(token.getType() != TokenTypes::LAMBDA_ARROW) {
            return std::move(parameter);
        }

        // If the token is a lambda arrow, pop the token from the lexer to proceed with parsing the lambda expression.
        simple_lambda_or_id_expr_lexer().pop();

        // Attempt to parse the body of the lambda expression.
        auto expr_res = static_cast<T*>(this)->expression();

        // If the body of the lambda expression couldn't be parsed, return the error.
        if(not expr_res) {
            return expr_res;
        }

        // If the body was successfully parsed, proceed with constructing the lambda expression.
        auto expr = std::move(expr_res.value());

        // Get the start and end text areas of the lambda expression.
        auto start = parameter.getArea();
        auto end = ast::getTextArea(expr);

        // Combine the start and end text areas into a single text area representing the whole lambda expression.
        auto area = lexing::TextArea::combine(start, end);

        // Construct a vector of parameters for the lambda expression.
        // Currently, only the initial identifier is used as a parameter.
        std::vector<ast::LambdaParameter> parameters;
        parameters.emplace_back(std::move(parameter));

        // Return the constructed lambda expression.
        return ast::Expression{
            // The lambda expression is constructed with its text area, the parameters, and the parsed body.
            // ast::forward is used to move the constructed lambda expression to the Expression variant type.
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
