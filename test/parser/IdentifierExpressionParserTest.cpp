#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto identifier_test_positive(std::string_view text, std::string_view expct)
{
	ast::Identifier expexted{{0, 0}, expct};

	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	ASSERT_TRUE(std::holds_alternative<ast::Identifier>(result.value()));

	EXPECT_EQ(std::get<ast::Identifier>(result.value()), expexted);
}

auto identifier_test_negative(std::string_view text)
{
	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	EXPECT_FALSE(std::holds_alternative<ast::Identifier>(result.value()));
}

TEST(IdentifierExpressionParserTest, IdentifierExpressionParsingPositiveTest)
{
	identifier_test_positive("hello", "hello");
	identifier_test_positive("a", "a");
	identifier_test_positive("x", "x");
	identifier_test_positive("xasdas", "xasdas");
	identifier_test_positive("_xasdas", "_xasdas");
	identifier_test_positive("_xasdas_", "_xasdas_");
	identifier_test_positive("asd2_", "asd2_");
	identifier_test_positive("(hello)", "hello");
	identifier_test_positive("((hello))", "hello");
}

TEST(IdentifierExpressionParserTest, IdentifierExpressionParsingNegativeTest)
{
	identifier_test_negative("hello=>a");
	identifier_test_negative("(hello,hello)");
	identifier_test_negative("self");
}
