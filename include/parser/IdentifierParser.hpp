#pragma once

#include <ast/common/Identifier.hpp>
#include <ast/Ast.hpp>
#include <common/Error.hpp>
#include <exception>
#include <expected>
#include <lexer/Lexer.hpp>
#include <lexer/Tokens.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

template<class T>
class IdentifierParser
{
public:
    constexpr auto identifier() noexcept
        -> std::expected<ast::Identifier, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;
        using ast::Identifier;

        auto result = identifier_lexer().peek_and_pop();
        if(not result.has_value()) {
            return std::unexpected(result.error());
        }

        auto token = std::move(result.value());

        if(token.getType() == TokenTypes::IDENTIFIER) {
            return Identifier{token.getArea(), token.getValue()};
        }

        UnexpectedToken error{token.getType(),
                              token.getArea(),
                              TokenTypes::IDENTIFIER};
        return std::unexpected(std::move(error));
    }

private:
    constexpr auto identifier_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
