#include "ast/expression/IfExpr.hpp"
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

auto if_(auto condition, auto then, auto else_) -> ast::Expression
{
    std::vector<ast::ElifExpr> v;
    return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
                                     std::move(condition),
                                     std::move(then),
                                     std::move(v),
                                     std::move(else_));
}

auto if_(auto condition, auto then, std::vector<ast::ElifExpr>&& elifs, auto else_) -> ast::Expression
{
    return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
                                     std::move(condition),
                                     std::move(then),
                                     std::move(elifs),
                                     std::move(else_));
}

auto elif_(auto condition, auto expr) -> ast::ElifExpr
{
    return ast::ElifExpr{lexing::TextArea{0, 0},
                         std::move(condition),
                         std::move(expr)};
}

inline auto mul(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Multiplication>(lexing::TextArea{0, 0},
                                          std::move(lhs),
                                          std::move(rhs))};
}

inline auto div(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::Division>(lexing::TextArea{0, 0},
                                    std::move(lhs),
                                    std::move(rhs))};
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

auto if_test_positive(std::string_view text, auto&& expected)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    ASSERT_TRUE(std::holds_alternative<ast::Forward<ast::IfExpr>>(result.value()));

    EXPECT_EQ((result.value()), expected);
}

auto if_test_negative(std::string_view text)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::IfExpr>>(result.value()));
}

TEST(IfExpressionParserTest, IfExpressionParsingPositiveTest)
{
    if_test_positive("if(a) b else c",
                     if_(id("a"), id("b"), id("c")));

    if_test_positive("if(a + b) c - d else c",
                     if_(add(id("a"), id("b")),
                         sub(id("c"), id("d")),
                         id("c")));

    if_test_positive("if(a) if(b) c else d else c",
                     if_(id("a"),
                         if_(id("b"),
                             id("c"),
                             id("d")),
                         id("c")));

    if_test_positive("if(a) if(b) c else d else c",
                     if_(id("a"),
                         if_(id("b"),
                             id("c"),
                             id("d")),
                         id("c")));

    if_test_positive("if(a) if(b) c else d else if(x) y else z",
                     if_(id("a"),
                         if_(id("b"),
                             id("c"),
                             id("d")),
                         if_(id("x"),
                             id("y"),
                             id("z"))));
}
TEST(IfExpressionParserTest, IfExpressionParsingElifPositiveTest)
{
    // Single 'elif'
    std::vector<ast::ElifExpr> elifs1;
    elifs1.push_back(elif_(id("c"), id("d")));
    if_test_positive("if(a) b elif(c) d else e",
                     if_(id("a"), id("b"),
                         std::move(elifs1),
                         id("e")));

    // Multiple 'elif'
    std::vector<ast::ElifExpr> elifs2;
    elifs2.push_back(elif_(id("c"), id("d")));
    elifs2.push_back(elif_(id("e"), id("f")));
    if_test_positive("if(a) b elif(c) d elif(e) f else g",
                     if_(id("a"), id("b"),
                         std::move(elifs2),
                         id("g")));

    // 'elif' with complex expressions
    std::vector<ast::ElifExpr> elifs3;
    elifs3.push_back(elif_(mul(id("e"), id("f")),
                           div(id("g"), id("h"))));
    if_test_positive("if(a + b) c - d elif(e * f) g / h else i",
                     if_(add(id("a"), id("b")), sub(id("c"), id("d")),
                         std::move(elifs3),
                         id("i")));
}

TEST(IfExpressionParserTest, IfExpressionParsingNestedIfPositiveTest)
{

    // Testing multiple nested if
    if_test_positive("if(a) if(b) c else if(d) e else x else f",
                     if_(id("a"),
                         if_(id("b"), id("c"),
                             if_(id("d"), id("e"), id("x"))),
                         id("f")));
}

TEST(IfExpressionParserTest, IfExpressionParsingComplexTest)
{
    std::vector<ast::ElifExpr> elifs;
    elifs.push_back(elif_(mul(id("b"), id("c")), add(id("d"), id("e"))));

    // Testing complex 'if' and 'elif' expression
    if_test_positive("if(a * b) c + d elif(b * c) d + e else f",
                     if_(mul(id("a"), id("b")), add(id("c"), id("d")),
                         std::move(elifs),
                         id("f")));
}

TEST(IfExpressionParserTest, IfExpressionParsingNestedElifPositiveTest)
{
    std::vector<ast::ElifExpr> elifs;
    elifs.emplace_back(elif_(id("b"), if_(id("c"), id("d"), id("x"))));
    // Testing 'elif' with nested 'if'
    if_test_positive("if(a) b elif(b) if(c) d else x else e",
                     if_(id("a"), id("b"),
                         std::move(elifs),
                         id("e")));
}

TEST(IfExpressionParserTest, IfExpressionParsingNegativeTest)
{
    if_test_negative("hello=>a");
    if_test_negative("(hello,hello)");
    if_test_negative("self");
}
