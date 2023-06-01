#pragma once

#include "lexer/TextArea.hpp"
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

template<class T>
class BasicTypeParser
{
public:
    constexpr auto basic_type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using common::error::Error;
        using ast::Type;
        using lexing::TokenTypes;

        if(basic_type_lexer().next_is(TokenTypes::L_PARANTHESIS)) {
            return static_cast<T *>(this)->par_type();
        }
        if(basic_type_lexer().next_is(TokenTypes::SELF_TYPE)) {
            return static_cast<T *>(this)->self_type();
        }

        if(basic_type_lexer().next_is(TokenTypes::IDENTIFIER)) {
            return static_cast<T *>(this)->named_type();
        }

        return basic_type_lexer()
            .peek_and_pop()
            .and_then(
                [this](auto token) -> std::expected<Type, Error> {
                    UnexpectedToken error{token.getType(),
                                          token.getArea(),
                                          TokenTypes::IDENTIFIER,
                                          TokenTypes::SELF_TYPE,
                                          TokenTypes::L_PARANTHESIS};

                    return std::unexpected(std::move(error));
                });
    }

private:
    constexpr auto basic_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
