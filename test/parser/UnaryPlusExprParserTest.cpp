#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

inline auto pos(auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::UnaryPlus>(lexing::TextArea{0, 0},
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

inline auto access(auto lhs, auto rhs) -> ast::Expression
{
    return ast::Expression{
        ast::forward<ast::MemberAccess>(lexing::TextArea{0, 0},
                                        std::move(lhs),
                                        std::move(rhs))};
}

inline auto expr_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.expression();

    ASSERT_TRUE(!!result);
    EXPECT_EQ((result.value()), expected);
}

TEST(UnaryPlusExprParserTest, SimpleUnaryPlusExprParserTest)
{
    expr_test_positive("+a", pos(id("a")));
    expr_test_positive("+a - +a", sub(pos(id("a")), pos(id("a"))));
    expr_test_positive("++a", pos(pos(id("a"))));
    expr_test_positive("+a + b", add(pos(id("a")), id("b")));
    expr_test_positive("+(a + b)", pos(add(id("a"), id("b"))));
    expr_test_positive("+a.b", pos(access(id("a"), id("b"))));
}
