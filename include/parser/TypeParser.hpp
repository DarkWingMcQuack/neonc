#pragma once

#include "ast/type/SelfType.hpp"
#include "common/Error.hpp"
#include "lexer/Tokens.hpp"
#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <exception>
#include <lexer/Lexer.hpp>
#include <parser/IdentifierParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// TODO: discuss if tuple types should be (a & b & c) or (a, b, c)
template<class T>
class TypeParser
{
public:
    constexpr auto type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        auto start_type_res = [this] {
            if(type_lexer().next_is(TokenTypes::L_PARANTHESIS)) {
                return compound_type();
            }
            if(type_lexer().next_is(TokenTypes::SELF_TYPE)) {
                return self_type();
            }
            return named_type();
        }();

        if(not start_type_res) {
            auto lex_res = type_lexer().peek_and_pop();
            if(not lex_res.has_value()) {
                return std::unexpected(lex_res.error());
            }

            auto token = std::move(lex_res.value());

            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::IDENTIFIER,
                                  TokenTypes::SELF_TYPE,
                                  TokenTypes::L_PARANTHESIS};
            return std::unexpected(std::move(error));
        }

        auto start_type = std::move(start_type_res.value());

        return type(std::move(start_type));
    }

private:
    constexpr auto type(ast::Type&& already_parsed) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        if(type_lexer().next_is(lexing::TokenTypes::QUESTIONMARK)) {
            auto opt_res = optional_type(std::move(already_parsed));
            if(not opt_res) {
                return opt_res;
            }

            return type(std::move(opt_res.value()));
        }

        if(type_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            auto lambda_res = simple_lambda_type(std::move(already_parsed));
            if(not lambda_res) {
                return lambda_res;
            }

            return type(std::move(lambda_res.value()));
        }

        return std::move(already_parsed);
    }

    constexpr auto self_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        auto result = type_lexer().peek_and_pop();
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

    constexpr auto named_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        std::vector<ast::Identifier> names;

        auto first_identifier_res = static_cast<T*>(this)->identifier();
        if(not first_identifier_res) {
            return std::unexpected(std::move(first_identifier_res.error()));
        }

        names.emplace_back(std::move(first_identifier_res.value()));

        while(type_lexer().pop_next_is(lexing::TokenTypes::COLON_COLON)) {
            auto identifier_res = static_cast<T*>(this)->identifier();
            if(not identifier_res) {
                return std::unexpected(std::move(identifier_res.error()));
            }
            names.emplace_back(std::move(identifier_res.value()));
        }

        auto area = lexing::TextArea::combine(names.front().getArea(),
                                              names.back().getArea());

        auto type_name = std::move(names.back());
        names.pop_back();

        return ast::Type{
            ast::NamedType{std::move(area),
                           std::move(names),
                           std::move(type_name)}};
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

    // only invoke if the next token is |
    constexpr auto union_type(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using lexing::TokenTypes;

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_OR)) {
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

        auto token_res = type_lexer().peek_and_pop();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        UnexpectedToken error{token.getType(),
                              token.getArea(),
                              TokenTypes::BITWISE_AND,
                              TokenTypes::BITWISE_OR,
                              TokenTypes::COMMA,
                              TokenTypes::R_PARANTHESIS};
        return std::unexpected(std::move(error));
    }

    // only call if the next token is a ?
    constexpr auto optional_type(ast::Type&& first_type) noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using common::error::UnexpectedToken;

        auto token_res = type_lexer().peek();
        if(not token_res.has_value()) {
            return std::unexpected(std::move(token_res.error()));
        }
        auto token = std::move(token_res.value());

        if(token.getType() != TokenTypes::QUESTIONMARK) {
            UnexpectedToken error{token.getType(),
                                  token.getArea(),
                                  TokenTypes::QUESTIONMARK};
            return std::unexpected(std::move(error));
        }

        type_lexer().pop();

        auto start = ast::getTextArea(first_type);
        auto end = token.getArea();
        auto area = lexing::TextArea::combine(start, end);

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
