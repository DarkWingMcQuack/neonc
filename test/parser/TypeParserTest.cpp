#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>
#include <parser/TypeParser.hpp>

#include <gtest/gtest.h>

using parser::TypeParser;

auto named_type_test(std::string_view text, std::string_view expected_type, auto... other)
{
    std::vector<ast::Identifier> ns{ast::Identifier{{0, 0}, other}...};
    ast::Identifier type{{0, 0}, expected_type};

    ast::NamedType expexted{{0, 0}, std::move(ns), std::move(type)};

    lexing::Lexer lexer{text};
    auto result = TypeParser{text, std::move(lexer)}.type();

    ASSERT_TRUE(!!result);
    ASSERT_TRUE(std::holds_alternative<ast::NamedType>(result.value()));

    EXPECT_EQ(std::get<ast::NamedType>(result.value()), expexted);
}

TEST(TypeParserTest, NamedTypeParserTest)
{
    named_type_test("hello", "hello");
    named_type_test("(hello)", "hello");
    named_type_test("((hello))", "hello");
    named_type_test("(((hello)))", "hello");
    named_type_test("foo::bar::baz", "baz", "foo", "bar");
    named_type_test("(foo::bar::baz)", "baz", "foo", "bar");
    named_type_test("((foo::bar::baz))", "baz", "foo", "bar");
    named_type_test("foo::foo::foo", "foo", "foo", "foo");
    named_type_test("foo::foo::foo", "foo", "foo", "foo");
    named_type_test("a::b::c::d::e::f::g", "g", "a", "b", "c", "d", "e", "f");
    named_type_test("(a::b::c::d::e::f::g)", "g", "a", "b", "c", "d", "e", "f");
}
