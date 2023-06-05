#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <ast/type/SelfType.hpp>
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
 * @class OneArgLambdaTypeParser
 * @brief OneArgLambdaTypeParser is a template class for parsing single-argument lambda types, following the CRTP (Curiously Recurring Template Pattern).
 * @details The class provides the functionality for parsing single-argument lambda types and expects the derived class to provide implementations for optional_type() and the lexer_ member.
 */
template<class T>
class OneArgLambdaTypeParser
{
public:
    /**
     * @brief Tries to parse a single-argument lambda type.
     * @details Tries to parse a single-argument lambda type by first parsing an optional type and then checking for a "LAMBDA_ARROW" token.
     * If such a token is found, it means that the parsed optional type is the parameter type of a lambda function. The method then continues parsing the return type of the lambda function.
     *
     * The method starts by trying to parse an optional type. If this fails, it immediately returns with an error.
     * Then, it checks if the next token is a "LAMBDA_ARROW". If it is, it continues parsing the return type of the lambda function.
     * If not, it returns the parsed optional type (since it is not part of a lambda type).
     *
     * @return On success, returns an expected value containing the parsed Type. On failure, returns an expected value containing an error.
     */
    constexpr auto one_arg_lambda_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using lexing::TextArea;

        return static_cast<T *>(this)
            ->optional_type()
            .and_then(
                [this](auto &&param) -> std::expected<ast::Type, common::error::Error> {
                    // Check if the optional type is the parameter type of a lambda function
                    if(oal_type_lexer().pop_next_is(TokenTypes::LAMBDA_ARROW)) {

                        // If it is, continue parsing the return type of the lambda function
                        return static_cast<T *>(this)
                            ->one_arg_lambda_type()
                            .transform(
                                [&](auto &&ret) {
                                    auto area = TextArea::combine(ast::getTextArea(param),
                                                                  ast::getTextArea(ret));

                                    // Create a new ast::Type that represents the lambda function
                                    return ast::Type{
                                        forward<ast::LambdaType>(area,
                                                                 std::move(param),
                                                                 std::move(ret))};
                                });
                    }

                    // If not, return the parsed optional type
                    return param;
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
    constexpr auto oal_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
