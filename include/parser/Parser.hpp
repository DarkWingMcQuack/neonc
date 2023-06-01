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
#include <parser/types/BasicTypeParser.hpp>
#include <parser/types/NamedTypeParser.hpp>
#include <parser/types/OneArgLambdaTypeParser.hpp>
#include <parser/types/OptionalTypeParser.hpp>
#include <parser/types/ParanthesisTypeParser.hpp>
#include <parser/types/SelfTypeParser.hpp>
#include <parser/types/TupleTypeParser.hpp>
#include <parser/types/TypeParser.hpp>
#include <parser/types/UnionTypeParser.hpp>
#include <string_view>

namespace parser {

class Parser final : public SelfTypeParser<Parser>,
                     public NamedTypeParser<Parser>,
                     public BasicTypeParser<Parser>,
                     public UnionTypeParser<Parser>,
                     public TupleTypeParser<Parser>,
                     public ParanthesisTypeParser<Parser>,
                     public OneArgLambdaTypeParser<Parser>,
                     public OptionalTypeParser<Parser>,
                     public TypeParser<Parser>,
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
    friend class OptionalTypeParser<Parser>;
    friend class BasicTypeParser<Parser>;
    friend class ParanthesisTypeParser<Parser>;
    friend class OneArgLambdaTypeParser<Parser>;
    friend class UnionTypeParser<Parser>;
    friend class TupleTypeParser<Parser>;
    friend class NamedTypeParser<Parser>;
    friend class SelfTypeParser<Parser>;
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
