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

/**
 * @class BasicTypeParser
 * @brief BasicTypeParser is a template class for parsing basic types in a given language, following the CRTP (Curiously Recurring Template Pattern).
 * @details The class provides the basic functionality for parsing types and expects the derived class to provide implementations for par_type(), self_type(), named_type(), and the lexer_ member.
 */
template<class T>
class BasicTypeParser
{
public:
    /**
     * @brief Tries to parse a basic type.
     * @details Tries to parse either a parenthesized type, a self type, or a named type, based on the next token in the lexer.
     *          If none of these is possible, an UnexpectedToken error is returned.
     * @return On success, returns an expected value containing the parsed Type.
     *         On failure, returns an expected value containing an UnexpectedToken error.
     */
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
    /**
     * @brief Accesses the lexer.
     * @details Returns a reference to the lexer that is being used by the parser. The lexer is expected to be provided by the derived class.
     * @return Reference to the lexer.
     */
    constexpr auto basic_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
