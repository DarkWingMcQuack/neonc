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
class TypeParser
{
public:
    constexpr auto type() -> std::expected<ast::Type, common::error::Error>
    {
        return static_cast<T *>(this)->union_type();
    }
};

} // namespace parser
