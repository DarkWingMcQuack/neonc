#pragma once

#include <ast/Ast.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

class Parser
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
