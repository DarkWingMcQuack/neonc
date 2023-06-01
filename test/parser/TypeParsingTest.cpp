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

inline auto tupleT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    std::vector<ast::Type> types;
    (types.emplace_back(std::move(elems)), ...);

    return ast::forward<ast::TupleType>(lexing::TextArea{0, 0}, std::move(types));
}

inline auto unionT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    std::vector<ast::Type> types;
    (types.emplace_back(std::move(elems)), ...);

    return ast::forward<ast::UnionType>(lexing::TextArea{0, 0}, std::move(types));
}

inline auto namedT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    auto ns = std::vector<ast::Identifier>{ast::Identifier{{0, 0}, elems}...};
    auto last = ns.back();
    ns.pop_back();

    return ast::NamedType{{0, 0}, std::move(ns), std::move(last)};
}

inline auto selfT() -> ast::Type
{
    return ast::SelfType{{0, 0}};
}

inline auto lambdaT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) >= 2);

    std::vector<ast::Type> params;
    (params.emplace_back(std::move(elems)), ...);

    auto last = std::move(params.back());
    params.pop_back();

    return ast::Type{ast::forward<ast::LambdaType>(lexing::TextArea{0, 0}, std::move(params), std::move(last))};
}

TEST(TypeParsingTest, NamedTypeParsingTest)
{
    auto result = Parser{"a::b::c"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), namedT("a", "b", "c"));
}

TEST(TypeParsingTest, OptionalTypeParsingTest)
{
    auto result = Parser{"Self?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), optOf(selfT()));
}

TEST(TypeParsingTest, TupleTypeParsingTest)
{
    auto result = Parser{"(a&b&c)"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), tupleT(namedT("a"), namedT("b"), namedT("c")));
}

TEST(TypeParsingTest, UnionTypeParsingTest)
{
    auto result = Parser{"(a|b|c)"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), unionT(namedT("a"), namedT("b"), namedT("c")));
}

TEST(TypeParsingTest, LambdaTypeParsingTest)
{
    auto result = Parser{"(a, b)=>c"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), lambdaT(namedT("a"), namedT("b"), namedT("c")));
}

TEST(TypeParsingTest, SelfTypeParsingTest)
{
    auto result = Parser{"Self"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), selfT());
}

TEST(TypeParsingTest, ParenthesesTypeParsingTest)
{
    auto result = Parser{"((a))"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), namedT("a"));
}

TEST(TypeParsingTest, ComplexTypeParsingTest)
{
    auto result = Parser{"(((a|b)&Self?)|((a, b)=>c))=>d"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(),
              lambdaT(
                  unionT(
                      tupleT(
                          unionT(
                              namedT("a"),
                              namedT("b")),
                          optOf(selfT())),
                      lambdaT(
                          namedT("a"),
                          namedT("b"),
                          namedT("c"))),
                  namedT("d")));
}

TEST(TypeParsingTest, OptionalNamedTypeParsingTest)
{
    auto result = Parser{"a::b::c?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), optOf(namedT("a", "b", "c")));
}

TEST(TypeParsingTest, OptionalTupleTypeParsingTest)
{
    auto result = Parser{"(a&b&c)?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), optOf(tupleT(namedT("a"), namedT("b"), namedT("c"))));
}

TEST(TypeParsingTest, OptionalUnionTypeParsingTest)
{
    auto result = Parser{"(a|b|c)?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), optOf(unionT(namedT("a"), namedT("b"), namedT("c"))));
}

TEST(TypeParsingTest, OptionalLambdaTypeParsingTest)
{
    auto result = Parser{"((a, b)=>c)?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(), optOf(lambdaT(namedT("a"), namedT("b"), namedT("c"))));
}

TEST(TypeParsingTest, ComplexOptionalTypeParsingTest)
{
    auto result = Parser{"((((a|b)&Self?)|((a, b)=>c))=>d)?"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(),
              optOf(
                  lambdaT(
                      unionT(
                          tupleT(
                              unionT(
                                  namedT("a"),
                                  namedT("b")),
                              optOf(selfT())),
                          lambdaT(namedT("a"),
                                  namedT("b"),
                                  namedT("c"))),
                      namedT("d"))));
}

TEST(TypeParsingTest, NestedLambdaTypeParsingTest)
{
    auto result = Parser{"((a, b)=>(c, d)=>e)"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(),
              lambdaT(
                  namedT("a"),
                  namedT("b"),
                  lambdaT(
                      namedT("c"),
                      namedT("d"),
                      namedT("e"))));
}

TEST(TypeParsingTest, ComplexUnionTypeParsingTest)
{
    auto result = Parser{"(a|(b&c)|(d, e)=>f)"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(),
              unionT(
                  namedT("a"),
                  tupleT(
                      namedT("b"),
                      namedT("c")),
                  lambdaT(namedT("d"),
                          namedT("e"),
                          namedT("f"))));
}

TEST(TypeParsingTest, ComplexTupleTypeParsingTest)
{
    auto result = Parser{"(a&(b|c)&(d, e)=>f)"}.type();
    ASSERT_TRUE(!!result);
    EXPECT_EQ(result.value(),
              tupleT(
                  namedT("a"),
                  unionT(
                      namedT("b"),
                      namedT("c")),
                  lambdaT(
                      namedT("d"),
                      namedT("e"),
                      namedT("f"))));
}
