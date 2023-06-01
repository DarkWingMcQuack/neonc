#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <ast/type/SelfType.hpp>
#include <common/Error.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <lexer/Tokens.hpp>
#include <parser/IdentifierParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// TODO: discuss if tuple types should be (a & b & c) or (a, b, c)
template<class T>
class SelfTypeParser
{
public:
    constexpr auto self_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto result = self_type_lexer().peek_and_pop();
        if(not result.has_value()) {
            return std::unexpected(result.error());
        }

        auto token = std::move(result.value());
        if(token.getType() == TokenTypes::SELF_TYPE) {
            return ast::Type{ast::SelfType{token.getArea()}};
        }

        UnexpectedToken error{token.getType(),
                              token.getArea(),
                              TokenTypes::IDENTIFIER};
        return std::unexpected(std::move(error));
    }

private:
    constexpr auto self_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
