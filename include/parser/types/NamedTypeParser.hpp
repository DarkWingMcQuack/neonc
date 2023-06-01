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
class NamedTypeParser
{
public:
    constexpr auto named_type() noexcept -> std::expected<ast::Type, common::error::Error>
    {
        std::vector<ast::Identifier> names;

        auto first_identifier_res = static_cast<T *>(this)->identifier();
        if(not first_identifier_res) {
            return std::unexpected(std::move(first_identifier_res.error()));
        }

        names.emplace_back(std::move(first_identifier_res.value()));

        while(named_type_lexer().pop_next_is(lexing::TokenTypes::COLON_COLON)) {
            auto identifier_res = static_cast<T *>(this)->identifier();
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

private:
    constexpr auto named_type_lexer() noexcept -> lexing::Lexer &
    {
        return static_cast<T *>(this)->lexer_;
    }
};
} // namespace parser
