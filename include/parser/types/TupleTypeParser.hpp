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
class TupleTypeParser
{
public:
    constexpr auto tuple_type() -> std::expected<ast::Type, common::error::Error>
    {
        using lexing::TokenTypes;
        using lexing::TextArea;

        auto first_type_res = static_cast<T *>(this)->one_arg_lambda_type();
        if(not first_type_res) {
            // If parsing the first type failed, propagate the error
            return first_type_res;
        }

        std::vector<ast::Type> types;
        types.emplace_back(std::move(first_type_res.value()));

        // Continue parsing the union type until no more '&' is encountered
        while(tuple_type_lexer().pop_next_is(lexing::TokenTypes::BITWISE_AND)) {
            auto next_type = static_cast<T *>(this)->one_arg_lambda_type();
            if(not next_type) {
                // If parsing the next type failed, propagate the error
                return next_type;
            }
            types.emplace_back(std::move(next_type.value()));
        }

        if(types.size() == 1) {
            return std::move(types.front());
        }

        auto area = TextArea::combine(ast::getTextArea(types.front()),
                                      ast::getTextArea(types.back()));

        // Return a new ast::Type that represents the parsed union type
        return ast::Type{
            forward<ast::TupleType>(std::move(area),
                                    std::move(types))};
    }

private:
    constexpr auto tuple_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
