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

template<class T>
class TypeParser
{
public:
    constexpr auto type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        return start()
            .and_then([this](auto&& t) {
                return type(std::move(t));
            });
    }

private:
    constexpr auto start() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using common::error::Error;
        using ast::Type;
        using lexing::TokenTypes;

        if(type_lexer().next_is(TokenTypes::L_PARANTHESIS)) {
            return compound_type();
        }
        if(type_lexer().next_is(TokenTypes::SELF_TYPE)) {
            return static_cast<T*>(this)->self_type();
        }

        if(type_lexer().next_is(TokenTypes::IDENTIFIER)) {
            return static_cast<T*>(this)->named_type();
        }

        return type_lexer()
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

    constexpr auto type(ast::Type&& already_parsed) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        if(type_lexer().next_is(lexing::TokenTypes::QUESTIONMARK)) {
            return optional_type(std::move(already_parsed))
                .and_then([this](auto&& t) {
                    return type(std::move(t));
                });
        }

        if(type_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            return simple_lambda_type(std::move(already_parsed))
                .and_then([this](auto&& t) {
                    return type(std::move(t));
                });
        }

        // if(type_lexer().next_is(lexing::TokenTypes::BITWISE_AND)) {
        //     return tuple_type(ast::getTextArea(already_parsed),
        //                       std::move(already_parsed))
        //         .and_then([this](auto&& t) {
        //             return type(std::move(t));
        //         });
        // }

        // if(type_lexer().next_is(lexing::TokenTypes::BITWISE_OR)) {
        //     return union_type(ast::getTextArea(already_parsed),
        //                       std::move(already_parsed))
        //         .and_then([this](auto&& t) {
        //             return type(std::move(t));
        //         });
        // }

        return std::move(already_parsed);
    }


    // only invoke if the next token is &
    constexpr auto tuple_type(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_AND)) {
            // parse the next type
            auto next_type = type();
            if(not next_type) {
                return next_type;
            }

            types.emplace_back(std::move(next_type.value()));
        }

        auto end_res = type_lexer().peek_and_pop();
        if(not end_res.has_value()) {
            return std::unexpected(std::move(end_res.error()));
        }

        auto end_token = std::move(end_res.value());
        if(end_token.getType() != TokenTypes::R_PARANTHESIS) {
            UnexpectedToken error{end_token.getType(),
                                  end_token.getArea(),
                                  TokenTypes::R_PARANTHESIS};
            return std::unexpected(std::move(error));
        }

        auto area = lexing::TextArea::combine(start, end_token.getArea());

        return ast::Type{
            forward<ast::TupleType>(std::move(area),
                                    std::move(types))};
    }

    /**
     * @brief This method is a part of the TypeParser class in the Neon programming language and is responsible for parsing union types.
     *
     * The Neon programming language allows a variable to be declared as a union type. A union type in Neon is a type that can hold values of
     * different types. Union types are specified using the syntax (Type1 | Type2 | ...), where Type1, Type2, etc. are the types that the union can hold.
     * For instance, a union type (Integer | String) can hold either an Integer or a String value.
     *
     * The method expects the type lexer to be positioned at a token representing the bitwise OR operator '|', which in Neon syntax indicates a
     * union type. If the token is not a '|', an error is returned.
     *
     * The method then combines the parsed types to form a new 'UnionType' abstract syntax tree node, which represents a union type in Neon's
     * internal representation of a program.
     *
     * @param[in] start The TextArea representing the starting location of the already parsed ( token in the source code.
     * @param[in] first_type The first type parsed inside the union's parentheses.
     *
     * @returns If parsing is successful, the method returns an expected type in the form of ast::Type holding the parsed union type.
     *          If an error occurs, such as when the next token is not a '|', it returns an UnexpectedToken error.
     *
     * @note This method should only be called if the next token in the lexer's token stream is a '|'.
     */
    constexpr auto union_type(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        // Continue parsing the union type until no more '|' is encountered
        while(type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_OR)) {
            auto next_type = type();
            if(not next_type) {
                // If parsing the next type failed, propagate the error
                return next_type;
            }
            types.emplace_back(std::move(next_type.value()));
        }

        // After parsing all types, the next token should be a right parenthesis ')'
        auto end_res = type_lexer().peek_and_pop();
        if(not end_res.has_value()) {
            // If peeking at the next token failed, propagate the error
            return std::unexpected(std::move(end_res.error()));
        }

        auto end_token = std::move(end_res.value());
        if(end_token.getType() != TokenTypes::R_PARANTHESIS) {
            // If the token is not a ')', create an error indicating that a ')' was expected
            UnexpectedToken error{end_token.getType(),
                                  end_token.getArea(),
                                  TokenTypes::R_PARANTHESIS};
            return std::unexpected(std::move(error));
        }

        auto area = lexing::TextArea::combine(start, end_token.getArea());

        // Return a new ast::Type that represents the parsed union type
        return ast::Type{
            forward<ast::UnionType>(std::move(area),
                                    std::move(types))};
    }


    constexpr auto lambda_type_with_multiple(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(TokenTypes::COMMA)) {
            // parse the next type
            auto next_type = type();
            if(not next_type.has_value()) {
                return next_type;
            }

            types.emplace_back(std::move(next_type.value()));
        }

        // next token should be a )
        auto token_res = type_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            return std::unexpected(token_res.error());
        }
        auto token = std::move(token_res.value());

        if(token.getType() != TokenTypes::R_PARANTHESIS) {
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::R_PARANTHESIS};
            return std::unexpected(std::move(error));
        }

        // next token should be a =>
        token_res = type_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            return std::unexpected(token_res.error());
        }
        token = std::move(token_res.value());

        if(token.getType() != TokenTypes::LAMBDA_ARROW) {
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::LAMBDA_ARROW};
            return std::unexpected(std::move(error));
        }

        // parse return type after =>
        auto return_type_res = type();
        if(not return_type_res.has_value()) {
            return return_type_res;
        }

        auto return_type = std::move(return_type_res.value());

        auto area = lexing::TextArea::combine(start, ast::getTextArea(return_type));

        return ast::Type{
            forward<ast::LambdaType>(std::move(area),
                                     std::move(types),
                                     std::move(return_type))};
    }

    // only invoke if the next token is )
    constexpr auto grouped_type(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> ast::Type
    {
        auto end = type_lexer().peek_and_pop().value().getArea();
        auto new_area = lexing::TextArea::combine(start, end);
        ast::setTextArea(first_type, std::move(new_area));
        return first_type;
    }

    // a compound type is a type involving ()
    // like a tuple (a & b) a union (a | b) or a grouped type (a) or a lambda type with multiple parameters
    constexpr auto compound_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto start_res = type_lexer().peek_and_pop();
        if(not start_res.has_value()) {
            return std::unexpected(std::move(start_res.error()));
        }
        auto start_token = std::move(start_res.value());

        if(start_token.getType() != lexing::TokenTypes::L_PARANTHESIS) {
            UnexpectedToken error{start_token.getType(),
                                  start_token.getArea(),
                                  TokenTypes::L_PARANTHESIS};

            return std::unexpected(std::move(error));
        }
        // get the area of the ( token
        auto start_area = start_token.getArea();

        // parse the first type after the (
        auto first_type_res = type();
        if(not first_type_res.has_value()) {
            return first_type_res;
        }

        auto first_type = std::move(first_type_res.value());

        // the type is a grouped type
        if(type_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            return grouped_type(start_area, std::move(first_type));
        }

        // the type is a tuple
        if(type_lexer().next_is(lexing::TokenTypes::BITWISE_AND)) {
            return tuple_type(start_area, std::move(first_type));
        }

        // the type is a union
        if(type_lexer().next_is(lexing::TokenTypes::BITWISE_OR)) {
            return union_type(start_area, std::move(first_type));
        }

        // the type is a lambda tpye with multiple parameters
        if(type_lexer().next_is(lexing::TokenTypes::COMMA)) {
            return lambda_type_with_multiple(start_area, std::move(first_type));
        }

        // if we are here, we have an error
        // to be valid we should have read & or | or , or )
        // so we get the token and create an error

        return type_lexer()
            .peek_and_pop()
            .and_then(
                [this](auto&& token) -> std::expected<ast::Type, common::error::Error> {
                    UnexpectedToken error{token.getType(),
                                          token.getArea(),
                                          TokenTypes::BITWISE_AND,
                                          TokenTypes::BITWISE_OR,
                                          TokenTypes::COMMA,
                                          TokenTypes::R_PARANTHESIS};

                    return std::unexpected(std::move(error));
                });
    }

    /**
     * @brief This method is a part of the TypeParser class in the Neon programming language and is responsible for parsing optional types.
     *
     * The Neon programming language allows a variable to be declared as an optional type. This optional typing mechanism means a variable can hold
     * either a value of a specific type or a special value called 'None'. This introduces a level of safety, allowing developers to handle potential
     * null values in a structured way. For example, an optional integer type in Neon can hold an integer value or None, making it clear that the
     * variable might not always hold a valid integer.
     *
     * The method expects the type lexer to be positioned at a token representing the question mark symbol '?', which in Neon syntax indicates that
     * a type is optional. If the token is not a question mark, an error is returned.
     *
     * The method then combines the text area of the question mark and the preceding type to form a new 'OptionalType' abstract syntax tree node,
     * which represents an optional type in the Neon programming language's internal representation of a program.
     *
     * @param[in] first_type The type to be parsed as optional. This type precedes the question mark in Neon's optional type syntax.
     *
     * @returns If parsing is successful, the function returns an expected type in the form of ast::Type holding the parsed optional type.
     *          If an error occurs, such as when the next token is not a question mark, it returns an UnexpectedToken error.
     *
     * @note This method should only be called if the next token in the lexer's token stream is a question mark.
     */
    constexpr auto optional_type(ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        // Attempt to peek at the next token without removing it from the lexer
        auto token_res = type_lexer().peek();
        if(not token_res.has_value()) {
            // If peeking at the next token failed, propagate the error
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        // Check that the token is a '?', as expected for an optional type
        if(token.getType() != TokenTypes::QUESTIONMARK) {
            // If the token is not a '?', create an error indicating that a '?' was expected
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::QUESTIONMARK};
            return std::unexpected(std::move(error));
        }

        // Consume the '?' token now that we have confirmed it is the correct type
        type_lexer().pop();

        // The text area of the optional type spans from the start of the first_type to the end of the '?'
        auto start = ast::getTextArea(first_type);
        auto end = token.getArea();
        auto area = lexing::TextArea::combine(start, end);

        // Return a new ast::Type that represents the optional type
        return ast::Type{
            forward<ast::OptionalType>(std::move(area),
                                       std::move(first_type))};
    }

    // only call if the next token is a =>
    constexpr auto simple_lambda_type(ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        // pop the =>
        type_lexer().pop();

        auto ret_type_res = type();

        if(not ret_type_res.has_value()) {
            return ret_type_res;
        }

        auto ret_type = std::move(ret_type_res.value());

        auto area = lexing::TextArea::combine(ast::getTextArea(first_type),
                                              ast::getTextArea(ret_type));

        return ast::Type{
            forward<ast::LambdaType>(area,
                                     std::move(first_type),
                                     std::move(ret_type))};
    }

private:
    constexpr auto type_lexer() noexcept -> lexing::Lexer&
    {
        return static_cast<T*>(this)->lexer_;
    }
};
} // namespace parser
