#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/SimpleExpressionParser.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// expression and statements need to be parsed together
// because an blockexpr or a if expr contains statements
// and on the other side statements contain expressions ofc
class ExpressionAndStatementParser : public TypeParser,
                                     SimpleExpressionParser<ExpressionAndStatementParser>
{
public:
    constexpr explicit ExpressionAndStatementParser(lexing::Lexer& lexer) noexcept
        : TypeParser(lexer) {}

    constexpr auto expression() noexcept -> std::optional<ast::Expression>;
    constexpr auto statement() noexcept -> std::optional<ast::Statement>;

private:
};

} // namespace parser
