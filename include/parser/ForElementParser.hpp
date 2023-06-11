#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <common/Error.hpp>
#include <lexer/Lexer.hpp>
#include <lexer/Tokens.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

/**
 * @class ForElementParser
 *
 * @tparam T The type of the derived class; this class utilizes the Curiously Recurring Template Pattern (CRTP).
 *
 * @brief A parser for handling 'for' loop elements in the abstract syntax tree.
 *
 * This class is designed to parse elements inside a 'for' loop. It can handle two kinds of elements:
 * 1. Assign elements, e.g. `a = 10`.
 * 2. Monadic elements, e.g. `a <- 10`.
 */
template<class T>
class ForElementParser
{
public:
    /**
     * @brief Parse a 'for' loop element.
     *
     * This method attempts to parse a 'for' loop element, which can be either an assign element
     * or a monadic element.
     *
     * @return A std::expected holding the parsed ast::ForElement or an error.
     */
    constexpr auto for_element() noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        // Try to parse an identifier
        auto id_res = static_cast<T*>(this)->identifier();
        if(not id_res.has_value()) {
            // Return error if parsing identifier failed
            return std::unexpected(std::move(id_res.error()));
        }
        auto id = std::move(id_res.value());

        // Peek at the next token to determine the element type
        auto token_res = for_element_lexer().peek();
        if(not token_res.has_value()) {
            // Return error if peeking failed
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        // If the token is '<-', it's a monadic element
        if(token.getType() == TokenTypes::L_ARROW) {
            return monadic_for_element(std::move(id));
        }

        // If the token is '=', it's an assign element
        if(token.getType() == TokenTypes::ASSIGN) {
            return let_for_element(std::move(id));
        }

        // Return an error for an unexpected token
        UnexpectedToken error{
            token.getType(),
            token.getArea(),
            TokenTypes::L_ARROW,
            TokenTypes::ASSIGN};

        return std::unexpected(std::move(error));
    }
    /**
     * @brief Parse a monadic 'for' loop element.
     *
     * This method attempts to parse a monadic 'for' loop element in the form `identifier <- expression`.
     *
     * @param id The parsed identifier before the '<-' token.
     * @return A std::expected holding the parsed ast::ForMonadicElement or an error.
     */
    constexpr auto monadic_for_element(ast::Identifier&& id) noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::InternalCompilerError;
        using lexing::TokenTypes;

        // Expect the '<-' token
        auto token_res = for_element_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            // Return error if peeking failed
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        // Return an error if the token is not '<-'
        if(token.getType() != TokenTypes::L_ARROW) {
            return std::unexpected(InternalCompilerError{});
        }

        // Parse the expression after '<-'
        auto rhs_res = static_cast<T*>(this)->expression();
        if(not rhs_res.has_value()) {
            // Return error if parsing expression failed
            return std::unexpected(std::move(rhs_res.error()));
        }
        auto rhs = std::move(rhs_res.value());

        // Create and return the monadic element
        auto start = id.getArea();
        auto end = ast::getTextArea(rhs);
        auto area = lexing::TextArea::combine(start, end);

        return ast::ForMonadicElement{std::move(area),
                                      std::move(id),
                                      std::move(rhs)};
    }

    /**
     * @brief Parse an assign 'for' loop element.
     *
     * This method attempts to parse an assign 'for' loop element in the form `identifier = expression`.
     *
     * @param id The parsed identifier before the '=' token.
     * @return A std::expected holding the parsed ast::ForLetElement or an error.
     */
    constexpr auto let_for_element(ast::Identifier&& id) noexcept
        -> std::expected<ast::ForElement, common::error::Error>
    {
        using common::error::InternalCompilerError;
        using lexing::TokenTypes;

        // Expect the '=' token
        auto token_res = for_element_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            // Return error if peeking failed
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        // Return an error if the token is not '='
        if(token.getType() != TokenTypes::ASSIGN) {
            return std::unexpected(common::error::InternalCompilerError{});
        }

        // Parse the expression after '='
        auto rhs_res = static_cast<T*>(this)->expression();
        if(not rhs_res.has_value()) {
            // Return error if parsing expression failed
            return std::unexpected(std::move(rhs_res.error()));
        }
        auto rhs = std::move(rhs_res.value());

        // Create and return the assign element
        auto start = id.getArea();
        auto end = ast::getTextArea(rhs);
        auto area = lexing::TextArea::combine(start, end);

        return ast::ForLetElement{std::move(area),
                                  std::move(id),
                                  std::move(rhs)};
    }

private:
    /**
     * @brief Obtain the lexer instance.
     *
     * This method returns a reference to the lexer used to tokenize the input stream.
     *
     * @return A reference to the lexer instance.
     */
    constexpr auto for_element_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};

} // namespace parser
