#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/BlockExpressionParser.hpp>
#include <parser/ExpressionPrattParser.hpp>
#include <parser/ForElementParser.hpp>
#include <parser/IfExpressionParser.hpp>
#include <parser/LParExpressionParser.hpp>
#include <parser/LetStmtParser.hpp>
#include <parser/SimpleExpressionParser.hpp>
#include <parser/SimpleLambdaOrIdentifierParser.hpp>
#include <parser/StatementParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class Parser final : public TypeParser<Parser>,
                     public SimpleExpressionParser<Parser>,
                     public StatmentParser<Parser>,
                     public IfExpressionParser<Parser>,
                     public BlockExpressionParser<Parser>,
                     public SimpleLambdaOrIdentifierParser<Parser>,
                     public LParExpressionParser<Parser>,
                     public ForElementParser<Parser>,
                     public ExpressionPrattParser<Parser>,
                     public LetStmtParser<Parser>,
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


private:
    friend class TypeParser<Parser>;
    friend class SimpleExpressionParser<Parser>;
    friend class StatmentParser<Parser>;
    friend class ForElementParser<Parser>;
    friend class IdentifierParser<Parser>;
    friend class IfExpressionParser<Parser>;
    friend class ExpressionPrattParser<Parser>;
    friend class BlockExpressionParser<Parser>;
    friend class LParExpressionParser<Parser>;
    friend class SimpleLambdaOrIdentifierParser<Parser>;
    friend class LetStmtParser<Parser>;

    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
