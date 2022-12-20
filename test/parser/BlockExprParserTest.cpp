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
