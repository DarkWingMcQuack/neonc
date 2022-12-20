#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto integer_test_positive(std::string_view text, std::int64_t expct)
{
    ast::Integer expexted{{0, 0}, expct};

    auto result = Parser{text}.simple_expression();

    ASSERT_TRUE(!!result);
    ASSERT_TRUE(std::holds_alternative<ast::Integer>(result.value()));

    EXPECT_EQ(std::get<ast::Integer>(result.value()), expexted);
}

auto integer_test_negative(std::string_view text)
{
    auto result = Parser{text}.simple_expression();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Integer>(result.value()));
}

TEST(IntegerExpressionParserTest, IntegerExpressionParsingPositiveTest)
{
    for(std::int64_t i = 0; i < 10000; i++) {
        std::int64_t n = rand();
        for(std::size_t j = 0; j < 10; j++) {
            std::string s;

            for(std::size_t k = 0; k < j; k++)
                s += "(";

            s += std::to_string(n);

            for(std::size_t k = 0; k < j; k++)
                s += ")";

            integer_test_positive(s, n);
        }
    }
}

TEST(IntegerExpressionParserTest, IntegerExpressionParsingZeroTest)
{
    std::string s;
    for(int i = 1; i < 1000; i++) {
        s += "0";
        integer_test_positive(s, 0);
    }
}

TEST(IntegerExpressionParserTest, IntegerExpressionParsingNegativeTest)
{
    integer_test_negative("hello=>a");
    integer_test_negative("(hello,hello)");
    integer_test_negative("self");
    integer_test_negative("x");
    integer_test_negative("xasdas");
    integer_test_negative("_xasdas");
    integer_test_negative("_xasdas_");
    integer_test_negative("asd2_");
    integer_test_negative("(hello)");
    integer_test_negative("((hello))");
}
