#pragma once

#include "common/Error.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
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
class ForElementParser
{
public:
    constexpr auto for_element() noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto id_res = static_cast<T*>(this)->identifier();
        if(not id_res.has_value()) {
            return std::unexpected(std::move(id_res.error()));
        }
        auto id = std::move(id_res.value());

        auto token_res = for_element_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() == TokenTypes::L_ARROW) {
            return monadic_for_element(std::move(id));
        }

        if(token.getType() == TokenTypes::ASSIGN) {
            return let_for_element(std::move(id));
        }


        UnexpectedToken error{
            token.getType(),
            token.getArea(),
            TokenTypes::L_ARROW,
            TokenTypes::ASSIGN};

        return std::unexpected(std::move(error));
    }

    constexpr auto monadic_for_element(ast::Identifier&& id) noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::InternalCompilerError;
        using lexing::TokenTypes;

        auto token_res = for_element_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() != TokenTypes::L_ARROW) {
            return std::unexpected(InternalCompilerError{});
        }

        auto rhs_res = static_cast<T*>(this)->expression();
        if(not rhs_res.has_value()) {
            return std::unexpected(std::move(rhs_res.error()));
        }
        auto rhs = std::move(rhs_res.value());
        auto start = id.getArea();
        auto end = ast::getTextArea(rhs);
        auto area = lexing::TextArea::combine(start, end);

        return ast::ForMonadicElement{std::move(area),
                                      std::move(id),
                                      std::move(rhs)};
    }

    constexpr auto let_for_element(ast::Identifier&& id) noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::InternalCompilerError;
        using lexing::TokenTypes;

        auto token_res = for_element_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() != TokenTypes::ASSIGN) {
            return std::unexpected(common::error::InternalCompilerError{});
        }

        auto rhs_res = static_cast<T*>(this)->expression();
        if(not rhs_res.has_value()) {
            return std::unexpected(std::move(rhs_res.error()));
        }

        auto rhs = std::move(rhs_res.value());
        auto start = id.getArea();
        auto end = ast::getTextArea(rhs);
        auto area = lexing::TextArea::combine(start, end);

        return ast::ForLetElement{std::move(area),
                                  std::move(id),
                                  std::move(rhs)};
    }

private:
    constexpr auto for_element_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
