#include <ast/expression/LambdaExpr.hpp>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>
#include <string_view>
#include <type_traits>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

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

inline auto add(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Addition>(lexing::TextArea{0, 0},
                                    std::move(lhs),
                                    std::move(rhs))};
}

inline auto sub(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Substraction>(lexing::TextArea{0, 0},
                                        std::move(lhs),
                                        std::move(rhs))};
}

inline auto mul(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Multiplication>(lexing::TextArea{0, 0},
                                          std::move(lhs),
                                          std::move(rhs))};
}

inline auto lambda(auto params, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::LambdaExpr>(lexing::TextArea{0, 0},
                                      std::move(params),
                                      std::move(rhs))};
}

inline auto params(auto... params) -> std::vector<ast::LambdaParameter>
{
    constexpr auto f = [](auto&& p) constexpr {
        if constexpr(std::is_same_v<std::decay_t<decltype(p)>, ast::LambdaParameter>) {
            return std::move(p);
        } else {
            return ast::LambdaParameter{id(p)};
        }
    };

    std::vector<ast::LambdaParameter> l;
    (l.emplace_back(f(params)), ...);
    return l;
}

inline auto param(std::string_view name, auto type) -> ast::LambdaParameter
{
    return ast::LambdaParameter{
        id(name),
        std::move(type)};
}



inline auto expr_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    EXPECT_EQ((result.value()), expected);
}

TEST(LambdaExprParserTest, SingleParamLambdaExprParserTest)
{
    expr_test_positive("a => i + j", lambda(params("a"), add(id("i"), id("j"))));
    expr_test_positive("b => (i + j)", lambda(params("b"), add(id("i"), id("j"))));
    expr_test_positive("c => i + j + k",
                       lambda(params("c"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("a => (i + j) + k",
                       lambda(params("a"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("a => i + j + k + l + m",
                       lambda(params("a"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}

TEST(LambdaExprParserTest, SingleTypedParamLambdaExprParserTest)
{
    expr_test_positive("(a: Int) => a", lambda(params(param("a", namedT("Int"))), id("a")));
}

TEST(LambdaExprParserTest, SingleParamWithParentesisLambdaExprParserTest)
{
    expr_test_positive("(a) => i + j", lambda(params("a"), add(id("i"), id("j"))));
    expr_test_positive("(b) => (i + j)", lambda(params("b"), add(id("i"), id("j"))));
    expr_test_positive("(c) => i + j + k",
                       lambda(params("c"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a) => (i + j) + k",
                       lambda(params("a"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a) => i + j + k + l + m",
                       lambda(params("a"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}

TEST(LambdaExprParserTest, DoubleParamLambdaExprParserTest)
{
    expr_test_positive("(a, b) => i + j", lambda(params("a", "b"), add(id("i"), id("j"))));
    expr_test_positive("(b, c) => (i + j)", lambda(params("b", "c"), add(id("i"), id("j"))));
    expr_test_positive("(c, d) => i + j + k",
                       lambda(params("c", "d"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, b) => (i + j) + k",
                       lambda(params("a", "b"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, xyz) => i + j + k + l + m",
                       lambda(params("a", "xyz"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}


TEST(LambdaExprParserTest, TripleParamLambdaExprParserTest)
{
    expr_test_positive("(a, b, c) => i + j", lambda(params("a", "b", "c"), add(id("i"), id("j"))));
    expr_test_positive("(b, c, d) => (i + j)", lambda(params("b", "c", "d"), add(id("i"), id("j"))));
    expr_test_positive("(c, d, e) => i + j + k",
                       lambda(params("c", "d", "e"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, b, someId) => (i + j) + k",
                       lambda(params("a", "b", "someId"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, xyz, HelloIamAnId) => i + j + k + l + m",
                       lambda(params("a", "xyz", "HelloIamAnId"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}
