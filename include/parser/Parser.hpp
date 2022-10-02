#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/ForElementParser.hpp>
#include <parser/SimpleExpressionParser.hpp>
#include <parser/IfExpressionParser.hpp>
#include <parser/StatementParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class Parser final : public TypeParser<Parser>,
                     public SimpleExpressionParser<Parser>,
                     public StatmentParser<Parser>,
                     public IfExpressionParser<Parser>,
                     public ForElementParser<Parser>,
                     public IdentifierParser<Parser>
{
public:
    constexpr explicit Parser(std::string_view content) noexcept
        : content_(content),
          lexer_(content_) {}

    constexpr Parser() noexcept = delete;
    constexpr Parser(Parser&&) noexcept = default;
    constexpr Parser(const Parser&) noexcept = delete;
    constexpr auto operator=(Parser&&) noexcept -> Parser& = default;
    constexpr auto operator=(const Parser&) noexcept -> Parser& = delete;
    constexpr virtual ~Parser() noexcept = default;

    constexpr auto expression() noexcept -> std::optional<ast::Expression>
    {
        return simple_expression();
    }


private:
    friend class TypeParser<Parser>;
    friend class SimpleExpressionParser<Parser>;
    friend class StatmentParser<Parser>;
    friend class ForElementParser<Parser>;
    friend class IdentifierParser<Parser>;
    friend class IfExpressionParser<Parser>;

    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
