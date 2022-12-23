#include <ast/expression/LambdaExpr.hpp>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>
#include <string_view>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
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

inline auto param(auto... params) -> std::vector<ast::LambdaParameter>
{
    std::vector<ast::LambdaParameter> l;
    (l.emplace_back(id(params)), ...);
    return l;
}



inline auto expr_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    EXPECT_EQ((result.value()), expected);
}

TEST(LambdaExprParserTest, SingleParamLambdaExprParserTest)
{
    expr_test_positive("a => i + j", lambda(param("a"), add(id("i"), id("j"))));
    expr_test_positive("b => (i + j)", lambda(param("b"), add(id("i"), id("j"))));
    expr_test_positive("c => i + j + k",
                       lambda(param("c"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("a => (i + j) + k",
                       lambda(param("a"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("a => i + j + k + l + m",
                       lambda(param("a"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}

TEST(LambdaExprParserTest, SingleParamWithParentesisLambdaExprParserTest)
{
    expr_test_positive("(a) => i + j", lambda(param("a"), add(id("i"), id("j"))));
    expr_test_positive("(b) => (i + j)", lambda(param("b"), add(id("i"), id("j"))));
    expr_test_positive("(c) => i + j + k",
                       lambda(param("c"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a) => (i + j) + k",
                       lambda(param("a"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a) => i + j + k + l + m",
                       lambda(param("a"),
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
    expr_test_positive("(a, b) => i + j", lambda(param("a", "b"), add(id("i"), id("j"))));
    expr_test_positive("(b, c) => (i + j)", lambda(param("b", "c"), add(id("i"), id("j"))));
    expr_test_positive("(c, d) => i + j + k",
                       lambda(param("c", "d"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, b) => (i + j) + k",
                       lambda(param("a", "b"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, xyz) => i + j + k + l + m",
                       lambda(param("a", "xyz"),
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
    expr_test_positive("(a, b, c) => i + j", lambda(param("a", "b", "c"), add(id("i"), id("j"))));
    expr_test_positive("(b, c, d) => (i + j)", lambda(param("b", "c", "d"), add(id("i"), id("j"))));
    expr_test_positive("(c, d, e) => i + j + k",
                       lambda(param("c", "d", "e"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, b, someId) => (i + j) + k",
                       lambda(param("a", "b", "someId"),
                              add(
                                  add(id("i"), id("j")),
                                  id("k"))));

    expr_test_positive("(a, xyz, HelloIamAnId) => i + j + k + l + m",
                       lambda(param("a", "xyz", "HelloIamAnId"),
                              add(
                                  add(
                                      add(
                                          add(id("i"), id("j")),
                                          id("k")),
                                      id("l")),
                                  id("m"))));
}
