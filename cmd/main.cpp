#include <ast/Ast.hpp>
#include <ast/expression/Integer.hpp>
#include <ctre/ctre.hpp>
#include <fmt/core.h>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Utils.hpp>

auto main(int argc, char *argv[]) -> int
{
    std::string s;
    fmt::print("enter a token text\n");
    std::getline(std::cin, s);

    lexing::Lexer l{s};

    static_assert(parser::parse_int("10").value() == 10);

    while(auto token = l.peek()) {
        if(token.value().getType() == lexing::TokenTypes::END_OF_FILE) {
            break;
        }

        fmt::print("{}\n", lexing::get_description(token.value().getType()));

        l.pop();
    }
}
