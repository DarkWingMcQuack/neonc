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

template<class T>
class OneArgLambdaTypeParser
{
public:
    constexpr auto one_arg_lambda_type() noexcept
        -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using lexing::TextArea;

        return static_cast<T *>(this)
            ->optional_type()
            .and_then(
                [this](auto &&param) -> std::expected<ast::Type, common::error::Error> {

                    if(oal_type_lexer().pop_next_is(TokenTypes::LAMBDA_ARROW)) {

                        return static_cast<T *>(this)
                            ->one_arg_lambda_type()
                            .transform(
                                [&](auto &&ret) {
                                    auto area = TextArea::combine(ast::getTextArea(param),
                                                                  ast::getTextArea(ret));

                                    return ast::Type{
                                        forward<ast::LambdaType>(area,
                                                                 std::move(param),
                                                                 std::move(ret))};
                                });
                    }

                    return param;
                });
    }

private:
    constexpr auto oal_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
