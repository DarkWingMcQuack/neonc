#include <ast/common/ForElements.hpp>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

inline auto if_(auto condition, auto then, auto else_) -> ast::Expression
{
    std::vector<ast::ElifExpr> v;
    return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
                                     std::move(condition),
                                     std::move(then),
                                     std::move(v),
                                     std::move(else_));
}

inline auto for_assign(std::string_view name, auto rhs) -> ast::ForElement
{
    return ast::forward<ast::ForLetElement>(lexing::TextArea{0, 0},
                                            id(name),
                                            std::move(rhs));
}
inline auto for_monadic(std::string_view name, auto rhs) -> ast::ForElement
{
    return ast::forward<ast::ForMonadicElement>(lexing::TextArea{0, 0},
                                                id(name),
                                                std::move(rhs));
}

auto test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.for_element();

    ASSERT_TRUE(!!result);
    EXPECT_EQ((result.value()), expected);
}


TEST(ForElemenentParserTest, PositiveLetTest)
{
    test_positive("hello = a", for_assign("hello", id("a")));
}
// Positive Test for Monadic For Element
TEST(ForElementParserTest, PositiveMonadicTest)
{
    test_positive("hello <- a", for_monadic("hello", id("a")));
    test_positive("foo <- bar", for_monadic("foo", id("bar")));
}

// Negative Test for Invalid Token
TEST(ForElementParserTest, NegativeInvalidTokenTest)
{
    auto result = Parser{"hello + a"}.for_element();

    ASSERT_FALSE(!!result);
}

// Negative Test for Missing Identifier
TEST(ForElementParserTest, NegativeMissingIdentifierTest)
{
    auto result = Parser{"= a"}.for_element();

    ASSERT_FALSE(!!result);
}

// Negative Test for Missing Expression in Assign Element
TEST(ForElementParserTest, NegativeMissingExpressionInAssignTest)
{
    auto result = Parser{"hello ="}.for_element();

    ASSERT_FALSE(!!result);
}

// Negative Test for Missing Expression in Monadic Element
TEST(ForElementParserTest, NegativeMissingExpressionInMonadicTest)
{
    auto result = Parser{"hello <-"}.for_element();

    ASSERT_FALSE(!!result);
}

// Positive Test for Complex Expression in Assign Element
TEST(ForElementParserTest, PositiveComplexExpressionInAssignTest)
{
    test_positive("hello = if(x) y else z", for_assign("hello", if_(id("x"), id("y"), id("z"))));
}

// Positive Test for Complex Expression in Monadic Element
TEST(ForElementParserTest, PositiveComplexExpressionInMonadicTest)
{
    test_positive("hello <- if(x) y else z", for_monadic("hello", if_(id("x"), id("y"), id("z"))));
}
