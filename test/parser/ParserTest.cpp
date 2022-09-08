#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto simple_id_test(std::string_view text) -> void
{
    auto result = Parser{text}.identifier();
    ASSERT_TRUE(!!result);

    auto id = result.value();

    EXPECT_EQ(id.getValue(), text);
}

TEST(ParserTest, SimpleIdentifierParsingTest)
{
    simple_id_test("asasas");
    simple_id_test("identifier");
    simple_id_test("iamanidentifier");
    simple_id_test("iamanidentifier_");
    simple_id_test("iamanidentifier_");
    simple_id_test("iamanidentifier_12");
    simple_id_test("a12");
}
