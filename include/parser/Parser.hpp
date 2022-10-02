#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/ForElementParser.hpp>
#include <parser/SimpleExpressionParser.hpp>
#include <parser/StatementParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class Parser : public TypeParser<Parser>,
               public SimpleExpressionParser<Parser>,
               public StatmentParser<Parser>,
               public ForElementParser<Parser>,
               public IdentifierParser<Parser>
{
public:
    constexpr Parser(std::string_view content) noexcept
        : content_(content),
          lexer_(content_) {}


private:
    std::string_view content_;
    lexing::Lexer lexer_;
};

} // namespace parser
