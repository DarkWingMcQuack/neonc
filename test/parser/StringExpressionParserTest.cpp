#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto string_test_positive(std::string_view text, std::string_view expct)
{
	ast::String expexted{{0, 0}, expct};

	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	ASSERT_TRUE(std::holds_alternative<ast::String>(result.value()));

	EXPECT_EQ(std::get<ast::String>(result.value()), expexted);
}

auto string_test_negative(std::string_view text)
{
	auto result = Parser{text}.simple_expression();

	ASSERT_TRUE(!!result);
	EXPECT_FALSE(std::holds_alternative<ast::String>(result.value()));
}

inline auto random_str(std::size_t len) noexcept -> std::string
{
	constexpr std::string_view alphanum =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		" \t";

	std::string tmp_s;
	tmp_s.reserve(len);

	for(std::size_t i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (alphanum.size() - 1)];
	}

	return tmp_s;
}



TEST(StringExpressionParserTest, StringExpressionParsingPositiveTest)
{
	for(std::int64_t i = 0; i < 9999; i++) {
		for(std::size_t j = 0; j < 10; j++) {
			std::string s;

			auto str = "\"" + random_str(i) + "\"";

			for(std::size_t k = 0; k < j; k++)
				s += "(";

			s += str;

			for(std::size_t k = 0; k < j; k++)
				s += ")";

			string_test_positive(s, str);
		}
	}
}

TEST(StringExpressionParserTest, StringExpressionParsingNegativeTest)
{
	string_test_negative("hello=>a");
	string_test_negative("(\"ello\",hello)");
	string_test_negative("self");
	string_test_negative("x");
	string_test_negative("xasdas");
	string_test_negative("_xasdas");
	string_test_negative("_xasdas_");
	string_test_negative("asd2_");
	string_test_negative("(hello)");
	string_test_negative("(10)");
	string_test_negative("10");
	string_test_negative("a => \"asd\"");
	string_test_negative("((hello))");
}
