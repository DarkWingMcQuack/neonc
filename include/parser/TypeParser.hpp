#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
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
        -> std::optional<ast::Type>
    {
        auto start_type_opt = [this] {
            if(type_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
                return compound_type();
            }
            if(type_lexer().next_is(lexing::TokenTypes::SELF_TYPE)) {
                return self_type();
            }
            return named_type();
        }();

        if(not start_type_opt) {
            // TODO: return error "expected Identifier or ( or Self"
            return std::nullopt;
        }
        auto start_type = std::move(start_type_opt.value());

        return type(std::move(start_type));
    }

private:
    constexpr auto type(ast::Type&& already_parsed) noexcept
        -> std::optional<ast::Type>
    {
        if(type_lexer().next_is(lexing::TokenTypes::QUESTIONMARK)) {
            auto opt_opt = optional_type(std::move(already_parsed));
            if(not opt_opt) {
                // TODO: propagate error
                return std::nullopt;
            }

            return type(std::move(opt_opt.value()));
        }

        if(type_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
            auto lambda_opt = simple_lambda_type(std::move(already_parsed));
            if(not lambda_opt) {
                // TODO: propagate error
                return std::nullopt;
            }

            return type(std::move(lambda_opt.value()));
        }

        return std::make_optional(std::move(already_parsed));
    }

    constexpr auto self_type() noexcept
        -> std::optional<ast::Type>
    {
        if(type_lexer().next_is(lexing::TokenTypes::SELF_TYPE)) {
            auto area = type_lexer().peek_and_pop().value().getArea();
            return ast::Type{ast::SelfType{area}};
        }

        return std::nullopt;
    }

    constexpr auto named_type() noexcept
        -> std::optional<ast::Type>
    {
        std::vector<ast::Identifier> names;

        auto first_identifier_opt = static_cast<T*>(this)->identifier();
        if(not first_identifier_opt) {
            return std::nullopt;
        }

        names.emplace_back(std::move(first_identifier_opt.value()));

        while(type_lexer().pop_next_is(lexing::TokenTypes::COLON_COLON)) {
            auto identifier_opt = static_cast<T*>(this)->identifier();
            if(not identifier_opt) {
                // TODO: return expexted Identifier error
                return std::nullopt;
            }
            names.emplace_back(std::move(identifier_opt.value()));
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
        -> std::optional<ast::Type>
    {
        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_AND)) {
            // parse the next type
            auto next_type = type();

            if(not next_type) {
                // TODO: return error explaining what happend
                return std::nullopt;
            }

            types.emplace_back(std::move(next_type.value()));
        }

        if(not type_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // return next token should be closing parentesis or type
            return std::nullopt;
        }

        auto end = type_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);

        return ast::Type{
            forward<ast::TupleType>(std::move(area),
                                    std::move(types))};
    }

    // only invoke if the next token is |
    constexpr auto union_type(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::optional<ast::Type>
    {
        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_OR)) {
            // parse the next type
            auto next_type = type();

            if(not next_type) {
                // TODO: return error explaining what happend
                return std::nullopt;
            }

            types.emplace_back(std::move(next_type.value()));
        }

        if(not type_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
            // return next token should be closing parentesis or type
            return std::nullopt;
        }

        auto end = type_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);

        return ast::Type{
            forward<ast::UnionType>(std::move(area),
                                    std::move(types))};
    }

    constexpr auto lambda_type_with_multiple(lexing::TextArea start, ast::Type&& first_type) noexcept
        -> std::optional<ast::Type>
    {
        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type));

        while(type_lexer().pop_next_is(lexing::TokenTypes::COMMA)) {
            // parse the next type
            auto next_type = type();

            if(not next_type) {
                // TODO: return error explaining what happend
                return std::nullopt;
            }

            types.emplace_back(std::move(next_type.value()));
        }

        if(type_lexer().peek_and_pop().value().getType() != lexing::TokenTypes::R_PARANTHESIS) {
            // TODO: return error next token should be )
            return std::nullopt;
        }

        if(type_lexer().peek_and_pop().value().getType() != lexing::TokenTypes::LAMBDA_ARROW) {
            // TODO: return error next token should be =>
            return std::nullopt;
        }

        auto return_type_opt = type();

        if(not return_type_opt) {
            // TODO: propagate error
            return std::nullopt;
        }

        auto return_type = std::move(return_type_opt.value());

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
        -> std::optional<ast::Type>
    {
        if(not type_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
            return std::nullopt;
        }
        // get the area of the ( token
        auto start_area = type_lexer().peek_and_pop().value().getArea();

        // parse the first type after the (
        auto first_type_opt = type();

        if(not first_type_opt) {
            // TODO: propagate the parsing error up
            return std::nullopt;
        }

        auto first_type = std::move(first_type_opt.value());

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

        // TODO return error "expected & or | or ) or ,"
        return std::nullopt;
    }

    // only call if the next token is a ?
    constexpr auto optional_type(ast::Type&& first_type) noexcept
        -> std::optional<ast::Type>
    {
        if(not type_lexer().next_is(lexing::TokenTypes::QUESTIONMARK)) {
            return std::nullopt;
        }

        auto start = ast::getTextArea(first_type);
        auto end = type_lexer().peek_and_pop().value().getArea();
        auto area = lexing::TextArea::combine(start, end);

        return ast::Type{
            forward<ast::OptionalType>(std::move(area),
                                       std::move(first_type))};
    }

    // only call if the next token is a =>
    constexpr auto simple_lambda_type(ast::Type&& first_type) noexcept
        -> std::optional<ast::Type>
    {
        // pop the =>
        type_lexer().pop();

        auto ret_type_opt = type();

        if(not ret_type_opt) {
            // TODO: propagate error
            return std::nullopt;
        }

        auto ret_type = std::move(ret_type_opt.value());

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
