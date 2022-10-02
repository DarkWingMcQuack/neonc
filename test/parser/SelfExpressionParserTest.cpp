#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto self_test_positive(std::string_view text)
{
	ast::SelfExpr expexted{{0, 0}};

	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	ASSERT_TRUE(std::holds_alternative<ast::SelfExpr>(result.value()));

	EXPECT_EQ(std::get<ast::SelfExpr>(result.value()), expexted);
}

auto self_test_negative(std::string_view text)
{
	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	EXPECT_FALSE(std::holds_alternative<ast::SelfExpr>(result.value()));
}


TEST(SelfExpressionParserTest, SelfExpressionParsingTest)
{
	std::string b = "self";
	for(std::size_t j = 0; j < 10; j++) {
		std::string s;

		for(std::size_t k = 0; k < j; k++)
			s += "(";

		s += b;

		for(std::size_t k = 0; k < j; k++)
			s += ")";

		self_test_positive(s);
	}
}

TEST(SelfExpressionParserTest, SelfExpressionParsingNegativeTest)
{
	self_test_negative("hello=>a");
	self_test_negative("(\"ello\",hello)");
	self_test_negative("x");
	self_test_negative("xasdas");
	self_test_negative("_xasdas");
	self_test_negative("_xasdas_");
	self_test_negative("asd2_");
	self_test_negative("(hello)");
	self_test_negative("(10)");
	self_test_negative("10");
	self_test_negative("a => \"asd\"");
	self_test_negative("((hello))");
}
