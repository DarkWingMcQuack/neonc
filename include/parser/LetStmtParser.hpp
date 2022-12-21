#pragma once

#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <common/Error.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for let statements
template<class T>
class LetStmtParser
{
public:
    constexpr auto let() noexcept -> std::expected<ast::Statement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto token_res = let_stmt_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() != TokenTypes::LET) {
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::LET};

            return std::unexpected(std::move(error));
        }
        let_stmt_lexer().pop();

        // get the area of the let token as start
        auto start = token.getArea();

        auto id_res = static_cast<T*>(this)->identifier();
        if(not id_res.has_value()) {
            return std::unexpected(std::move(id_res.error()));
        }
        auto id = std::move(id_res.value());

        // parse optional type annotation
        std::optional<ast::Type> t;
        if(let_stmt_lexer().next_is(lexing::TokenTypes::COLON)) {
            let_stmt_lexer().pop();

            auto type_res = static_cast<T*>(this)->type();
            if(not type_res.has_value()) {
                return std::unexpected(std::move(type_res.error()));
            }
            t = std::move(type_res.value());
        }

        auto assign_token_res = let_stmt_lexer().peek_and_pop();
        if(not assign_token_res.has_value()) {
            return std::unexpected(std::move(assign_token_res.error()));
        }
        auto assign_token = std::move(assign_token_res.value());

        if(assign_token.getType() != TokenTypes::ASSIGN) {
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::ASSIGN};

            return std::unexpected(std::move(error));
        }

        auto rhs_res = static_cast<T*>(this)->expression();
        if(not rhs_res.has_value()) {
            return rhs_res;
        }
        auto rhs = std::move(rhs_res.value());

        auto end = ast::getTextArea(rhs);
        auto area = lexing::TextArea::combine(start, end);

        return ast::Statement{
            ast::forward<ast::LetAssignment>(area,
                                             std::move(id),
                                             std::move(t),
                                             std::move(rhs))};
    }

private:
    constexpr auto let_stmt_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
