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

/**
 * @class NamedTypeParser
 * @brief NamedTypeParser is a template class for parsing named types in a given language, following the CRTP (Curiously Recurring Template Pattern).
 * @details The class provides the functionality for parsing named types and expects the derived class to provide implementations for identifier() and the lexer_ member.
 */
template<class T>
class NamedTypeParser
{
public:
    /**
     * @brief Tries to parse a named type.
     * @details Tries to parse a named type by expecting a series of identifiers separated by "::".
     *          If any identifier cannot be parsed, an error is returned.
     *
     * The method starts by trying to parse the first identifier. If this fails, it immediately returns with an error.
     * Then, it enters a loop where it checks if the next token is a "::". If it is, it tries to parse another identifier.
     * If any identifier cannot be parsed, it returns with an error.
     * Once no more "::" tokens are found, it constructs a NamedType AST node from the parsed identifiers and returns it wrapped in a Type AST node.
     *
     * @return On success, returns an expected value containing the parsed Type. On failure, returns an expected value containing an error.
     */
    constexpr auto named_type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        // Stores the identifiers that make up the named type
        std::vector<ast::Identifier> names;

        // Try to parse the first identifier
        auto first_identifier_res = static_cast<T *>(this)->identifier();
        if(not first_identifier_res) {
            // If the parsing fails, return the error
            return std::unexpected(std::move(first_identifier_res.error()));
        }

        // If the parsing succeeds, add the identifier to the names vector
        names.emplace_back(std::move(first_identifier_res.value()));

        // Continue parsing identifiers as long as "::" tokens are found
        while(named_type_lexer().pop_next_is(lexing::TokenTypes::COLON_COLON)) {
            // Try to parse the next identifier
            auto identifier_res = static_cast<T *>(this)->identifier();
            if(not identifier_res) {
                // If the parsing fails, return the error
                return std::unexpected(std::move(identifier_res.error()));
            }
            // If the parsing succeeds, add the identifier to the names vector
            names.emplace_back(std::move(identifier_res.value()));
        }

        // Create an area that spans from the start of the first identifier to the end of the last identifier
        auto area = lexing::TextArea::combine(names.front().getArea(),
                                              names.back().getArea());

        // The type name is the last identifier
        auto type_name = std::move(names.back());
        names.pop_back();

        // Create and return a NamedType AST node wrapped in a Type AST node
        return ast::Type{
            ast::NamedType{std::move(area),
                           std::move(names),
                           std::move(type_name)}};
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
    constexpr auto named_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
