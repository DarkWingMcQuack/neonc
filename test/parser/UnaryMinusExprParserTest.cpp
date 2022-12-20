#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
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

TEST(UnaryMinusExprParserTest, SimpleUnaryMinusExprParserTest)
{
    expr_test_positive("-a", neg(id("a")));
    expr_test_positive("--a", neg(neg(id("a"))));
    expr_test_positive("-a + b", add(neg(id("a")), id("b")));
    expr_test_positive("-a + -b", add(neg(id("a")), neg(id("b"))));
    expr_test_positive("-(a + b)", neg(add(id("a"), id("b"))));
    expr_test_positive("-a.b", neg(access(id("a"), id("b"))));
}
