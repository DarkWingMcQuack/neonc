#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto named_type_test(std::string_view text, std::string_view expected_type, auto... other)
{
    std::vector<ast::Identifier> ns{ast::Identifier{{0, 0}, other}...};
    ast::Identifier type{{0, 0}, expected_type};

    ast::NamedType expexted{{0, 0}, std::move(ns), std::move(type)};

    auto result = Parser{text}.named_type();
    ASSERT_TRUE(!!result);

    EXPECT_EQ(result.value(), expexted);
}

TEST(TypeParserTest, NamedTypeParserTest)
{
    named_type_test("hello", "hello");
    named_type_test("foo::bar::baz", "baz", "foo", "bar");
    named_type_test("foo::foo::foo", "foo", "foo", "foo");
    named_type_test("foo::foo::foo", "foo", "foo", "foo");
    named_type_test("a::b::c::d::e::f::g", "g", "a", "b", "c", "d", "e", "f");

	//whitespace not allowed in named_type
	EXPECT_FALSE(Parser{"foo:: bar::baz"}.named_type());
	//empty identifier not allowed
	EXPECT_FALSE(Parser{"foo::::baz"}.named_type());
}
