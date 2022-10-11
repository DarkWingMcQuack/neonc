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

inline auto neg(auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::UnaryMinus>(lexing::TextArea{0, 0},
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

auto if_(auto condition, auto then, auto else_) -> ast::Expression
{
	std::vector<ast::ElifExpr> v;
	return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
									 std::move(condition),
									 std::move(then),
									 std::move(v),
									 std::move(else_));
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

inline auto access(auto lhs, auto rhs) -> ast::Expression
{
	return ast::Expression{
		ast::forward<ast::MemberAccess>(lexing::TextArea{0, 0},
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

TEST(FunctionCallExprParserTest, ComplexArgumentFunctionCallExprParsingTest)
{
	expr_test_positive("i(i + j)", call(id("i"), add(id("i"), id("j"))));
	expr_test_positive("i(i() + i())", call(id("i"), add(call(id("i")), call(id("i")))));
	expr_test_positive("i() + j()", add(call(id("i")), call(id("j"))));
}


TEST(FunctionCallExprParserTest, AccessFunctionCallExprParsingTest)
{
	expr_test_positive("i.j()", call(access(id("i"), id("j"))));
	expr_test_positive("i.j.k()", call(access(access(id("i"), id("j")), id("k"))));
	expr_test_positive("-i.j.k()", neg(call(access(access(id("i"), id("j")), id("k")))));
}


TEST(FunctionCallExprParserTest, LambdaFunctionCallExprParsingTest)
{
	expr_test_positive("a => i()", lambda("a", call(id("i"))));
	expr_test_positive("a => i(j)", lambda("a", call(id("i"), id("j"))));
	expr_test_positive("(a => i)()", call(lambda("a", id("i"))));
}
