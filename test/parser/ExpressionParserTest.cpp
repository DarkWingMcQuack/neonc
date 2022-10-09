#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto id(std::string_view text) -> ast::Identifier
{
	return ast::Identifier{{0, 0}, text};
}

auto add(auto lhs, auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::Addition>(lexing::TextArea{0, 0},
									std::move(lhs),
									std::move(rhs))};
}

auto mul(auto lhs, auto rhs) -> ast::Expression
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


TEST(ExpressionParserTest, AdditionExpressionParsingTest)
{

  expr_test_positive("i + j", add(id("i"), id("j")));
  expr_test_positive("i + j + k", add(add(id("i"), id("j")), id("k")));

	// expr_test_positive("i + j * k", add(id("i"), mul(id("j"), id("k"))));

	// expr_test_positive("i + j + k", add(add(id("i"), id("j")), id("k")));
}
