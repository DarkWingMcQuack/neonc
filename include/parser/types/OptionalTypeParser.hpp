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

template<class T>
class OptionalTypeParser
{
public:
    constexpr auto optional_type() -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        return static_cast<T *>(this)
            ->basic_type()
            .transform([this](auto &&current) {
                auto start = ast::getTextArea(current);
                while(auto to = opt_type_lexer().pop_next_is(TokenTypes::QUESTIONMARK)) {
                    auto token = std::move(to.value());

                    auto end = token.getArea();
                    auto area = lexing::TextArea::combine(start, end);

                    // Return a new ast::Type that represents the optional type
                    current = ast::Type{
                        forward<ast::OptionalType>(std::move(area),
                                                   std::move(current))};
                }

                return current;
            });
    }

private:
    constexpr auto opt_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
