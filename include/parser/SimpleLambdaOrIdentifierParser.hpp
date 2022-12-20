#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
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
    constexpr auto identifier_or_simple_lambda() noexcept -> std::optional<ast::Expression>
    {

        if(auto result = static_cast<T*>(this)->identifier()) {
            return simple_lambda(std::move(result.value()));
        }

        return std::nullopt;
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
    constexpr auto simple_lambda(ast::Identifier&& parameter) noexcept -> std::optional<ast::Expression>
    {

        if(simple_lambda_or_id_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
            // TODO: return "lambda parameters with type annotations need to be enclosed by ()"
            return std::nullopt;
        }

        if(not simple_lambda_or_id_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            return std::move(parameter);
        }

        auto expr_opt = static_cast<T*>(this)->expression();
        if(not expr_opt) {
            // TODO: propagate error
            return std::nullopt;
        }
        auto expr = std::move(expr_opt.value());

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
