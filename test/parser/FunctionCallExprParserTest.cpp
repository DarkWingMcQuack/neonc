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

inline auto call(auto lhs, auto... rhs) -> ast::Expression
{
	std::vector<ast::Expression> params;
	(params.emplace_back(std::move(rhs)), ...);

	return ast::Expression{
		ast::forward<ast::FunctionCall>(lexing::TextArea{0, 0},
										std::move(lhs),
										std::move(params))};
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


TEST(FunctionCallExprParserTest, SimpleFunctionCallExprParsingTest)
{
	expr_test_positive("i()", call(id("i")));
	expr_test_positive("i(j)", call(id("i"), id("j")));
	expr_test_positive("i(j, j)", call(id("i"), id("j"), id("j")));

	expr_test_positive("i{}", call(id("i")));
	expr_test_positive("i{j}", call(id("i"), id("j")));
	expr_test_positive("i{j, j}", call(id("i"), id("j"), id("j")));
}
