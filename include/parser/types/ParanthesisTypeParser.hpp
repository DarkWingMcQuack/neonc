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
#include <vector>

namespace parser {

template<class T>
class ParanthesisTypeParser
{
public:
    constexpr auto par_type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using common::error::Error;
        using lexing::TokenTypes;
        using ast::Type;

        return parse_token(TokenTypes::L_PARANTHESIS)
            .and_then(
                [&](auto &&start_area) {
                    return parse_type_list()
                        .and_then(
                            [&](auto &&type_list) {
                                return parse_token(TokenTypes::R_PARANTHESIS)
                                    .transform(
                                        [&](auto /*token*/) {
                                            return std::move(type_list);
                                        });
                            })
                        .and_then(
                            [&](auto &&type_list) -> std::expected<Type, Error> {
                                if(type_list.size() == 1) {
                                    return std::move(type_list.front());
                                }

                                return parse_lambda_ret()
                                    .transform(
                                        [&](auto &&ret_type) {
                                            auto area = lexing::TextArea::combine(start_area, ast::getTextArea(ret_type));
                                            return ast::Type{
                                                ast::forward<ast::LambdaType>(area,
                                                                              std::move(type_list),
                                                                              std::move(ret_type))};
                                        });
                            });
                });
    }

private:
    constexpr auto parse_token(lexing::TokenTypes type) noexcept -> std::expected<lexing::TextArea, common::error::Error>
    {
        using common::error::UnexpectedToken;
        using common::error::Error;
        using lexing::TokenTypes;
        using lexing::TextArea;

        return par_type_lexer()
            .peek_and_pop()
            .and_then(
                [&](auto &&token) -> std::expected<TextArea, Error> {
                    if(token.getType() != type) {
                        UnexpectedToken error{token.getType(),
                                              token.getArea(),
                                              type};
                        return std::unexpected(std::move(error));
                    }

                    return token.getArea();
                });
    }

    constexpr auto parse_lambda_ret() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        return parse_token(lexing::TokenTypes::LAMBDA_ARROW)
            .and_then([this](auto /*token*/) {
                return static_cast<T *>(this)->optional_type();
            });
    }

    constexpr auto parse_type_list() noexcept -> std::expected<std::vector<ast::Type>, common::error::Error>

    {
        using lexing::TokenTypes;
        using lexing::TextArea;

        auto first_type_res = static_cast<T *>(this)->type();
        if(not first_type_res) {
            // If parsing the first type failed, propagate the error
            return std::unexpected(std::move(first_type_res.error()));
        }

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type_res.value()));

        // Continue parsing the union type until no more '&' is encountered
        while(par_type_lexer().pop_next_is(lexing::TokenTypes::COMMA)) {
            auto next_type = static_cast<T *>(this)->type();
            if(not next_type) {
                // If parsing the next type failed, propagate the error
                return std::unexpected(std::move(next_type.error()));
            }
            types.emplace_back(std::move(next_type.value()));
        }

        return types;
    }

private:
    constexpr auto par_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};

} // namespace parser
