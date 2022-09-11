#include <ast/Ast.hpp>
#include <ast/expression/Integer.hpp>
#include <ctre/ctre.hpp>
#include <fmt/core.h>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

auto main(int argc, char *argv[]) -> int
{
    static_assert(common::is_specialization_of<std::variant, std::variant<bool, int>>::value);
    std::string s;
    fmt::print("enter a token text\n");
    std::getline(std::cin, s);

    lexing::Lexer l{s};

    while(auto token = l.peek()) {
        if(token.value().getType() == lexing::TokenTypes::END_OF_FILE) {
            break;
        }

        fmt::print("{}\n", lexing::get_description(token.value().getType()));
        fmt::print("{}\n", token.value().getValue());

        l.pop();
    }
}
