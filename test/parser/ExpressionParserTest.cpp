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


TEST(ExpressionParserTest, SimpleAdditionExpressionParsingTest)
{
	expr_test_positive("i + j", add(id("i"), id("j")));
	expr_test_positive("i + j + k",
					   add(
						   add(id("i"), id("j")),
						   id("k")));
	expr_test_positive("i + j + k + l + m",
					   add(
						   add(
							   add(
								   add(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i + (j + k)",
					   add(id("i"),
						   add(id("j"), id("k"))));

	// expr_test_positive("i + j * k", add(id("i"), mul(id("j"), id("k"))));

	// expr_test_positive("i + j + k", add(add(id("i"), id("j")), id("k")));
}

TEST(ExpressionParserTest, SimpleSubstractionExpressionParsingTest)
{
	expr_test_positive("i - j", sub(id("i"), id("j")));
	expr_test_positive("i - j - k",
					   sub(
						   sub(id("i"), id("j")),
						   id("k")));
	expr_test_positive("i - j - k - l - m",
					   sub(
						   sub(
							   sub(
								   sub(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i - (j - k)",
					   sub(id("i"),
						   sub(id("j"), id("k"))));
}

TEST(ExpressionParserTest, SimpleAdditionSubstractionExpressionParsingTest)
{
	expr_test_positive("i - j", sub(id("i"), id("j")));
	expr_test_positive("i + j - k",
					   sub(
						   add(id("i"), id("j")),
						   id("k")));
	expr_test_positive("i + j - k + l + m",
					   add(
						   add(
							   sub(
								   add(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i + (j - k)",
					   add(id("i"),
						   sub(id("j"), id("k"))));
}


TEST(ExpressionParserTest, SimpleMultiplicationExpressionParsingTest)
{
	expr_test_positive("i * j", mul(id("i"), id("j")));
	expr_test_positive("i * j * k",
					   mul(
						   mul(id("i"), id("j")),
						   id("k")));
	expr_test_positive("i * j * k * l * m",
					   mul(
						   mul(
							   mul(
								   mul(id("i"), id("j")),
								   id("k")),
							   id("l")),
						   id("m")));

	expr_test_positive("i * (j * k)",
					   mul(id("i"),
						   mul(id("j"), id("k"))));
}

TEST(ExpressionParserTest, SimpleMultiplicationAdditionExpressionParsingTest)
{
	expr_test_positive("i * j", mul(id("i"), id("j")));
	expr_test_positive("i + j * k",
					   add(id("i"),
						   mul(id("j"),
							   id("k"))));
	expr_test_positive("i + j * k * l + m",
					   add(
						   add(
							   id("i"),
							   mul(
								   mul(id("j"),
									   id("k")),
								   id("l"))),
						   id("m")));

	expr_test_positive("i * (j + k)",
					   mul(id("i"),
						   add(id("j"), id("k"))));
}
