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

inline auto lambda(std::string_view param, auto rhs) -> ast::Expression
{
	ast::LambdaParameter par{ast::Identifier{{0, 0}, param}};
	std::vector<ast::LambdaParameter> params;
	params.emplace_back(std::move(par));

	return ast::Expression{
		ast::forward<ast::LambdaExpr>(lexing::TextArea{0, 0},
									  std::move(params),
									  std::move(rhs))};
}



inline auto expr_test_positive(std::string_view text, auto expected)
{
	auto result = Parser{text}.expression();

	ASSERT_TRUE(!!result);
	EXPECT_EQ((result.value()), expected);
}

TEST(LambdaExprParserTest, SingleParamLambdaExprParserTest)
{
	expr_test_positive("a => i + j", lambda("a", add(id("i"), id("j"))));
	expr_test_positive("b => (i + j)", lambda("b", add(id("i"), id("j"))));
	expr_test_positive("c => i + j + k",
					   lambda("c",
							  add(
								  add(id("i"), id("j")),
								  id("k"))));

	expr_test_positive("a => (i + j) + k",
					   lambda("a",
							  add(
								  add(id("i"), id("j")),
								  id("k"))));

	expr_test_positive("a => i + j + k + l + m",
					   lambda("a",
							  add(
								  add(
									  add(
										  add(id("i"), id("j")),
										  id("k")),
									  id("l")),
								  id("m"))));
}
