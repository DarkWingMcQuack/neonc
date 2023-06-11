#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto self_type_test_positive(std::string_view text)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<ast::SelfType>(result.value()));
}

auto self_type_test_negative(std::string_view text)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::NamedType>(result.value()));
}

TEST(SelfTypeParsingTest, SelfTypeParsingPositiveTest)
{
    self_type_test_positive("Self");
    self_type_test_positive("(Self)");
    self_type_test_positive("((Self))");
    self_type_test_positive("(((Self)))");
}

TEST(SelfTypeParsingTest, SelfTypeParsingNegativeTest)
{
    self_type_test_negative("hello?");
    self_type_test_negative("hello??");
    self_type_test_negative("hello=>hello");
    self_type_test_negative("hello=>hello=>hello");
    self_type_test_negative("(hello&hello)");
    self_type_test_negative("(hello|hello)");
    self_type_test_negative("(hello|hello)=>hello");
    self_type_test_negative("(Self)?");
    self_type_test_negative("Self?");
    self_type_test_negative("Self=>Self");
    self_type_test_negative("(Self=>Self)?");
}
