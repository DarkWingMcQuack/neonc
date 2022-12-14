#include <iostream>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;


auto identifier_test(std::string_view text, std::string_view expected_value)
{
    ast::Identifier expexted{{0, 0}, expected_value};

    lexing::Lexer lexer{text};

    auto result = Parser{text}.identifier();

    ASSERT_TRUE(!!result);

    EXPECT_EQ(result.value(), expexted);
}


TEST(IdentifierParserTest, SimpleIdentifierParserTest)
{
    identifier_test("hello", "hello");
    identifier_test("hello ", "hello");
    identifier_test("hello asd", "hello");
    identifier_test("hs_ello asd", "hs_ello");
    identifier_test("__ asd", "__");
}
