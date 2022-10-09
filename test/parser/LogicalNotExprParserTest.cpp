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
inline auto pos(auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::UnaryPlus>(lexing::TextArea{0, 0},
									 std::move(rhs))};
}
inline auto no(auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::LogicalNot>(lexing::TextArea{0, 0},
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

TEST(LogicalNotExprParserTest, SimpleLogicalNotExprParserTest)
{
	expr_test_positive("!a", no(id("a")));
	expr_test_positive("!!a", no(no(id("a"))));
	expr_test_positive("!-a", no(neg(id("a"))));
	expr_test_positive("-!a", neg(no(id("a"))));
	expr_test_positive("!a + b", add(no(id("a")), id("b")));
	expr_test_positive("!a + !b", add(no(id("a")), no(id("b"))));
	expr_test_positive("!a + +b", add(no(id("a")), pos(id("b"))));
	expr_test_positive("!a + -b", add(no(id("a")), neg(id("b"))));
	expr_test_positive("!(a + b)", no(add(id("a"), id("b"))));
	expr_test_positive("!a.b", no(access(id("a"), id("b"))));
}
