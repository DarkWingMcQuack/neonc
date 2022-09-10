#include <concepts>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>
#include <parser/TypeParser.hpp>

#include <gtest/gtest.h>

using parser::TypeParser;

auto optOf(auto type) -> ast::Type
{
    return ast::Type{
        std::make_unique<ast::OptionalType>(lexing::TextArea{0, 0},
                                            ast::Type{std::move(type)})};
}

auto namedT(auto... elems) -> ast::NamedType
{
    static_assert(sizeof...(elems) > 0);

    auto ns = std::vector<ast::Identifier>{ast::Identifier{{0, 0}, elems}...};
    auto last = ns.back();
    ns.pop_back();

    return ast::NamedType{{0, 0}, std::move(ns), std::move(last)};
}

auto selfT() -> ast::SelfType
{
    return ast::SelfType{{0, 0}};
}

auto optional_type_test_positive(std::string_view text, auto expected)
{
    lexing::Lexer lexer{text};
    auto result = TypeParser{text, std::move(lexer)}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<std::unique_ptr<ast::OptionalType>>(result.value()));
    EXPECT_EQ(*std::get<std::unique_ptr<ast::OptionalType>>(result.value()),
              *std::get<std::unique_ptr<ast::OptionalType>>(expected));
}




TEST(OptionalTypeParsingTest, OptionalTypeParsingPositiveTest)
{
    optional_type_test_positive("hello?", optOf(namedT("hello")));
    optional_type_test_positive("a::b::c?", optOf(namedT("a", "b", "c")));
    optional_type_test_positive("Self?", optOf(selfT()));
    // optional_type_test_positive("Self??", optOf(optOf(selfT())));
    // optional_type_test_positive("Self???", optOf(optOf(optOf(selfT()))));
}

TEST(OptionalTypeParsingTest, OptionalTypeParsingNegativeTest)
{
}
