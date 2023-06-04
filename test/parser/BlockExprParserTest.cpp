#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

inline auto let(auto name, auto rhs) -> ast::Statement
{
    return ast::forward<ast::LetAssignment>(lexing::TextArea{0, 0},
                                            std::move(name),
                                            std::nullopt,
                                            std::move(rhs));
}

inline auto block(auto e, auto... stmts) -> ast::Expression
{
    std::vector<ast::Statement> s;

    (s.emplace_back(std::move(stmts)),
     ...);

    return ast::Expression{
        ast::forward<ast::BlockExpr>(lexing::TextArea{0, 0},
                                     std::move(s),
                                     std::move(e))};
}

inline auto neg(auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::UnaryMinus>(lexing::TextArea{0, 0},
                                      std::move(rhs))};
}

inline auto add(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Addition>(lexing::TextArea{0, 0},
                                    std::move(lhs),
                                    std::move(rhs))};
}

inline auto access(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::MemberAccess>(lexing::TextArea{0, 0},
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

inline auto expr_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    EXPECT_EQ((result.value()), expected);
}

TEST(BlockExprParserTest, SingleExprBlockExprParserTest)
{

    expr_test_positive("{a}", block(id("a")));
    expr_test_positive("{a+b}", block(add(id("a"), id("b"))));
}

TEST(BlockExprParserTest, ZeroStmtsBlockExprParserTest)
{
    expr_test_positive("{=>a}", block(id("a")));
    expr_test_positive("{=>a+b}", block(add(id("a"), id("b"))));
}


TEST(BlockExprParserTest, BlockExprWithLetParserTest)
{

    expr_test_positive(
        "{"
        "let a = c;"
        "let b = d;"
        "=> a+b"
        "}",
        block(add(id("a"), id("b")),
              let(id("a"), id("c")),
              let(id("b"), id("d"))));

    expr_test_positive(
        "{"
        "let a = c\n"
        "let b = d\n"
        "=> a+b"
        "}",
        block(add(id("a"), id("b")),
              let(id("a"), id("c")),
              let(id("b"), id("d"))));

    expr_test_positive(
        "{"
        "let a = c\n"
        "let b = d;"
        "let b = d\n"
        "=> a+b"
        "}",
        block(add(id("a"), id("b")),
              let(id("a"), id("c")),
              let(id("b"), id("d")),
              let(id("b"), id("d"))));

    expr_test_positive(
        "{"
        "let a = c;"
        "let b = d\n"
        "let b = d;"
        "=> a+b"
        "}",
        block(add(id("a"), id("b")),
              let(id("a"), id("c")),
              let(id("b"), id("d")),
              let(id("b"), id("d"))));
}
TEST(BlockExprParserTest, ComplexExprBlockParserTest)
{
    // Test for blocks with complex expressions
    expr_test_positive(
        "{=>a*b+c}",
        block(add(mul(id("a"), id("b")), id("c"))));

    expr_test_positive(
        "{=>a.b*c+d}",
        block(add(mul(access(id("a"), id("b")), id("c")), id("d"))));
}

TEST(BlockExprParserTest, NestedBlockExprParserTest)
{
    // Test for blocks with nested blocks
    expr_test_positive(
        "{let a = {=>b+c}; => a*d}",
        block(mul(id("a"), id("d")),
              let(id("a"), block(add(id("b"), id("c"))))));

    expr_test_positive(
        "{let a = {let b = e; =>b+c}; => a*d}",
        block(mul(id("a"), id("d")),
              let(id("a"), block(add(id("b"), id("c")), let(id("b"), id("e"))))));
}

TEST(BlockExprParserTest, DifferentStatementsBlockExprParserTest)
{
    expr_test_positive(
        "{let a = c; let b = {=>d+e}; => a*b}",
        block(mul(id("a"), id("b")),
              let(id("a"), id("c")),
              let(id("b"), block(add(id("d"), id("e"))))));
}

TEST(BlockExprParserTest, UnaryOperatorExprBlockParserTest)
{
    // Test for blocks with unary operator expressions
    expr_test_positive(
        "{=>-a}",
        block(neg(id("a"))));

    expr_test_positive(
        "{let a = c; => -a}",
        block(neg(id("a")),
              let(id("a"), id("c"))));
}

TEST(BlockExprParserTest, MultiLevelNestedBlockExprParserTest)
{
    // Test for blocks with multi-level nested blocks
    expr_test_positive(
        "{let a = {let b = {=>c+d}; => b*e}; => a*f}",
        block(mul(id("a"), id("f")),
              let(id("a"), block(mul(id("b"), id("e")), let(id("b"), block(add(id("c"), id("d"))))))));

    expr_test_positive(
        "{let a = {let b = {=>c+d}\n => b*e}\n => a*f}",
        block(mul(id("a"), id("f")),
              let(id("a"), block(mul(id("b"), id("e")), let(id("b"), block(add(id("c"), id("d"))))))));
}

TEST(BlockExprParserTest, MemberAccessExprBlockParserTest)
{
    // Test for blocks with member access expressions
    expr_test_positive(
        "{=>a.b}",
        block(access(id("a"), id("b"))));

    expr_test_positive(
        "{let a = c; => a.d}",
        block(access(id("a"), id("d")),
              let(id("a"), id("c"))));

    expr_test_positive(
        "{let a = c\n => a.d}",
        block(access(id("a"), id("d")),
              let(id("a"), id("c"))));
}

TEST(BlockExprParserTest, ComplexMultiStatementBlockExprParserTest)
{
    // Test for blocks with multiple complex statements
    expr_test_positive(
        "{"
        "let a = c+d;"
        "let b = e.f*g;"
        "let c = {let d = h; =>d+i};"
        "=> a*b*c"
        "}",
        block(mul(mul(id("a"), id("b")), id("c")),
              let(id("a"), add(id("c"), id("d"))),
              let(id("b"), mul(access(id("e"), id("f")), id("g"))),
              let(id("c"), block(add(id("d"), id("i")), let(id("d"), id("h"))))));

    expr_test_positive(
        "{"
        "let a = c+d\n"
        "let b = e.f*g\n"
        "let c = {let d = h; =>d+i}\n"
        "=> a*b*c"
        "}",
        block(mul(mul(id("a"), id("b")), id("c")),
              let(id("a"), add(id("c"), id("d"))),
              let(id("b"), mul(access(id("e"), id("f")), id("g"))),
              let(id("c"), block(add(id("d"), id("i")), let(id("d"), id("h"))))));
}
