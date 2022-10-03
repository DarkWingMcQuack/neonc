#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto id(std::string_view text) -> ast::Identifier
{
	return ast::Identifier{{0, 0}, text};
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

auto if_test_positive(std::string_view text, auto expected)
{
	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	ASSERT_TRUE(std::holds_alternative<ast::Forward<ast::IfExpr>>(result.value()));

	EXPECT_EQ((result.value()), expected);
}

auto if_test_negative(std::string_view text)
{
	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::IfExpr>>(result.value()));
}

TEST(IfExpressionParserTest, IfExpressionParsingPositiveTest)
{
	if_test_positive("if(a) b else c", if_(id("a"), id("b"), id("c")));
	// clang-format off
	if_test_positive("if(a) if(b) c else d else c", if_(id("a"),
														if_(id("b"),
															id("c"),
															id("d")),
														id("c")));

	if_test_positive("if(a) if(b) c else d else if(x) y else z", if_(id("a"),
																	 if_(id("b"),
																		 id("c"),
																		 id("d")),
																	 if_(id("x"),
																		 id("y"),
																		 id("z"))));
	// clang-format on
}

TEST(IfExpressionParserTest, IfExpressionParsingNegativeTest)
{
	if_test_negative("hello=>a");
	if_test_negative("(hello,hello)");
	if_test_negative("self");
}
