#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <concepts>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto optOf(auto type) -> ast::Type
{
    return ast::Type{
        ast::forward<ast::OptionalType>(lexing::TextArea{0, 0},
                                        ast::Type{std::move(type)})};
}

inline auto namedT(auto... elems) -> ast::NamedType
{
    static_assert(sizeof...(elems) > 0);

    auto ns = std::vector<ast::Identifier>{ast::Identifier{{0, 0}, elems}...};
    auto last = ns.back();
    ns.pop_back();

    return ast::NamedType{{0, 0}, std::move(ns), std::move(last)};
}


inline auto selfT() -> ast::SelfType
{
    return ast::SelfType{{0, 0}};
}

inline auto lambdaT(auto... elems) -> ast::LambdaType
{
    static_assert(sizeof...(elems) >= 2);

    std::vector<ast::Type> params;
    (params.emplace_back(std::move(elems)), ...);

    auto last = std::move(params.back());
    params.pop_back();

    return ast::LambdaType{{0, 0}, std::move(params), std::move(last)};
}

inline auto optional_type_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<ast::Forward<ast::OptionalType>>(result.value()));
    EXPECT_EQ(result.value(), expected);
}

inline auto optional_type_test_negative(std::string_view text)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::OptionalType>>(result.value()));
}


TEST(OptionalTypeParsingTest, OptionalTypeParsingPositiveTest)
{
    optional_type_test_positive("hello?", optOf(namedT("hello")));
    optional_type_test_positive("a::b::c?", optOf(namedT("a", "b", "c")));
    optional_type_test_positive("Self?", optOf(selfT()));
    optional_type_test_positive("Self??", optOf(optOf(selfT())));
    optional_type_test_positive("Self???", optOf(optOf(optOf(selfT()))));
    optional_type_test_positive("(((Self)?)?)?", optOf(optOf(optOf(selfT()))));
    optional_type_test_positive("(hello=>hello)?", optOf(lambdaT(namedT("hello"), namedT("hello"))));
}

TEST(OptionalTypeParsingTest, OptionalTypeParsingNegativeTest)
{
    optional_type_test_negative("hello=>hello?");
    optional_type_test_negative("hello?=>hello?");
    optional_type_test_negative("hello");
    optional_type_test_negative("(a|b?)");
    optional_type_test_negative("(a?|b?)");
    optional_type_test_negative("(a|b::s?)");
    optional_type_test_negative("(a::b?|b::b?)");
    optional_type_test_negative("(a&b?)");
}
