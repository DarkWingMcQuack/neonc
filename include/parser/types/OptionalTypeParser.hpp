#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <ast/type/SelfType.hpp>
#include <codecvt>
#include <common/Error.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <parser/IdentifierParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

/**
 * @class OptionalTypeParser
 * @brief OptionalTypeParser is a template class for parsing optional types in a given language, following the CRTP (Curiously Recurring Template Pattern).
 * @details The class provides the functionality for parsing optional types and expects the derived class to provide implementations for basic_type() and the lexer_ member.
 */
template<class T>
class OptionalTypeParser
{
public:
    /**
     * @brief Tries to parse an optional type.
     * @details Tries to parse an optional type by first parsing a basic type and then checking for a sequence of "?" tokens.
     * Each "?" token makes the type optional one more time (e.g., "Type???" is equivalent to "Optional<Optional<Optional<Type>>>" in some languages).
     *
     * The method starts by trying to parse a basic type. If this fails, it immediately returns with an error.
     * Then, it enters a loop where it checks if the next token is a "?". Each "?" token found makes the type optional one more time.
     * Once no more "?" tokens are found, it returns the parsed type.
     *
     * @return On success, returns an expected value containing the parsed Type. On failure, returns an expected value containing an error.
     */
    constexpr auto optional_type() -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        return static_cast<T *>(this)
            ->basic_type()
            .transform([this](auto &&current) {
                auto start = ast::getTextArea(current);
                // Continue parsing "?" tokens as long as they are found
                while(auto to = opt_type_lexer().pop_next_is(TokenTypes::QUESTIONMARK)) {
                    auto token = std::move(to.value());

                    auto end = token.getArea();
                    auto area = lexing::TextArea::combine(start, end);

                    // Each "?" token found makes the type optional one more time
                    current = ast::Type{
                        forward<ast::OptionalType>(std::move(area),
                                                   std::move(current))};
                }

                return current;
            });
    }

private:
    /**
     * @brief Accesses the lexer.
     * @details Returns a reference to the lexer that is being used by the parser. The lexer is expected to be provided by the derived class.
     *
     * The lexer is used to read tokens from the input. The derived class is expected to have a member named lexer_ of type lexing::Lexer or compatible.
     *
     * @return Reference to the lexer.
     */
    constexpr auto opt_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
