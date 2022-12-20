#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto boolean_test_positive(std::string_view text, bool expct)
{
    ast::Boolean expexted{{0, 0}, expct};

    auto result = Parser{text}.simple_expression();

    ASSERT_TRUE(!!result);
    ASSERT_TRUE(std::holds_alternative<ast::Boolean>(result.value()));

    EXPECT_EQ(std::get<ast::Boolean>(result.value()), expexted);
}

auto boolean_test_negative(std::string_view text)
{
    auto result = Parser{text}.simple_expression();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Boolean>(result.value()));
}


TEST(BooleanExpressionParserTest, BooleanExpressionParsingTrueTest)
{
    std::string b = "true";
    for(std::size_t j = 0; j < 10; j++) {
        std::string s;

        for(std::size_t k = 0; k < j; k++)
            s += "(";

        s += b;

        for(std::size_t k = 0; k < j; k++)
            s += ")";

        boolean_test_positive(s, true);
    }
}

TEST(BooleanExpressionParserTest, BooleanExpressionParsingFalseTest)
{
    std::string b = "false";
    for(std::size_t j = 0; j < 10; j++) {
        std::string s;

        for(std::size_t k = 0; k < j; k++)
            s += "(";

        s += b;

        for(std::size_t k = 0; k < j; k++)
            s += ")";

        boolean_test_positive(s, false);
    }
}

TEST(BooleanExpressionParserTest, BooleanExpressionParsingNegativeTest)
{
    boolean_test_negative("hello=>a");
    boolean_test_negative("(\"ello\",hello)");
    boolean_test_negative("self");
    boolean_test_negative("x");
    boolean_test_negative("xasdas");
    boolean_test_negative("_xasdas");
    boolean_test_negative("_xasdas_");
    boolean_test_negative("asd2_");
    boolean_test_negative("(hello)");
    boolean_test_negative("(10)");
    boolean_test_negative("10");
    boolean_test_negative("a => \"asd\"");
    boolean_test_negative("((hello))");
}
