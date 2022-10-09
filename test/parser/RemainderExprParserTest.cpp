#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

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

inline auto div(auto lhs, auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::Division>(lexing::TextArea{0, 0},
									std::move(lhs),
									std::move(rhs))};
}

inline auto rem(auto lhs, auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::Remainder>(lexing::TextArea{0, 0},
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

auto expr_test_positive(std::string_view text, auto expected)
{
	auto result = Parser{text}.expression();

	ASSERT_TRUE(!!result);
	EXPECT_EQ((result.value()), expected);
}


TEST(RemainderExprParserTest, SimpleRemainderExprParserTest)
{
	expr_test_positive("i % j",
					   rem(id("i"),
						   id("j")));

	expr_test_positive("i % j % k",
					   rem(rem(id("i"),
							   id("j")),
						   id("k")));
	expr_test_positive("i % j % k % l % m",
					   rem(
						   rem(
							   rem(
								   rem(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("(i % j) % k % l % m",
					   rem(
						   rem(
							   rem(
								   rem(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("((((i % j) % k) % l) % m)",
					   rem(
						   rem(
							   rem(
								   rem(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i % (j % k)",
					   rem(id("i"),
						   rem(id("j"), id("k"))));
}

TEST(RemainderExprParserTest, SimpleRemMultExprParserTest)
{
	expr_test_positive("i % j * k",
					   mul(
						   rem(id("i"),
							   id("j")),
						   id("k")));
	expr_test_positive("i % j % k * l * m",
					   mul(
						   mul(
							   rem(
								   rem(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i * (j % k)",
					   mul(id("i"),
						   rem(id("j"), id("k"))));
}

TEST(RemainderExprParserTest, SimpleRemDivExprParserTest)
{
	expr_test_positive("i % j / k",
					   div(
						   rem(id("i"),
							   id("j")),
						   id("k")));
	expr_test_positive("i % j % k / l / m",
					   div(
						   div(
							   rem(
								   rem(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i / (j % k)",
					   div(id("i"),
						   rem(id("j"), id("k"))));
}

TEST(RemainderExprParserTest, SimpleRemAddExprParserTest)
{
	expr_test_positive("i + j % k",
					   add(id("i"),
						   rem(id("j"),
							   id("k"))));

	expr_test_positive("i + j % k % l + m",
					   add(
						   add(
							   id("i"),
							   rem(
								   rem(id("j"),
									   id("k")),
								   id("l"))),
						   id("m")));

	expr_test_positive("i % (j + k)",
					   rem(id("i"),
						   add(id("j"), id("k"))));
}


TEST(RemainderExprParserTest, SimpleRemSubExprParserTest)
{
	expr_test_positive("i - j % k",
					   sub(id("i"),
						   rem(id("j"),
							   id("k"))));

	expr_test_positive("i - j % k % l - m",
					   sub(
						   sub(
							   id("i"),
							   rem(
								   rem(id("j"),
									   id("k")),
								   id("l"))),
						   id("m")));

	expr_test_positive("i % (j - k)",
					   rem(id("i"),
						   sub(id("j"), id("k"))));
}
