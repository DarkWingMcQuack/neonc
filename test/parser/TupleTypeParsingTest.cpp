#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <concepts>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>
#include <parser/TypeParser.hpp>

#include <gtest/gtest.h>

using parser::TypeParser;

inline auto optOf(auto type) -> ast::Type
{
    return ast::Type{
        ast::forward<ast::OptionalType>(lexing::TextArea{0, 0},
                                        ast::Type{std::move(type)})};
}

inline auto tupleT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    std::vector<ast::Type> types;
    (types.emplace_back(std::move(elems)), ...);

    return ast::forward<ast::TupleType>(lexing::TextArea{0, 0}, std::move(types));
}

inline auto unionT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    std::vector<ast::Type> types;
    (types.emplace_back(std::move(elems)), ...);

    return ast::forward<ast::UnionType>(lexing::TextArea{0, 0}, std::move(types));
}

inline auto namedT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) > 0);

    auto ns = std::vector<ast::Identifier>{ast::Identifier{{0, 0}, elems}...};
    auto last = ns.back();
    ns.pop_back();

    return ast::NamedType{{0, 0}, std::move(ns), std::move(last)};
}

inline auto selfT() -> ast::Type
{
    return ast::SelfType{{0, 0}};
}

inline auto lambdaT(auto... elems) -> ast::Type
{
    static_assert(sizeof...(elems) >= 2);

    std::vector<ast::Type> params;
    (params.emplace_back(std::move(elems)), ...);

    auto last = std::move(params.back());
    params.pop_back();

    return ast::Type{ast::forward<ast::LambdaType>(lexing::TextArea{0, 0}, std::move(params), std::move(last))};
}

inline auto tuple_type_test_positive(std::string_view text, auto expected)
{
    lexing::Lexer lexer{text};
    auto result = TypeParser{lexer}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<ast::Forward<ast::TupleType>>(result.value()));
    EXPECT_EQ(result.value(), expected);
}

inline auto tuple_type_test_negative(std::string_view text)
{
    lexing::Lexer lexer{text};
    auto result = TypeParser{lexer}.type();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::TupleType>>(result.value()));
}


TEST(TupleTypeParsingTest, TupleTypeParsingPositiveTest)
{
    tuple_type_test_positive("(a&b)", tupleT(namedT("a"), namedT("b")));
    tuple_type_test_positive("(a&b&c)", tupleT(namedT("a"), namedT("b"), namedT("c")));
    tuple_type_test_positive("(a&b&c&d)", tupleT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    tuple_type_test_positive("((a&b&c&d))", tupleT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    tuple_type_test_positive("(((a&b&c&d)))", tupleT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    tuple_type_test_positive("((a&b&c&(d=>e)))", tupleT(namedT("a"), namedT("b"), namedT("c"), lambdaT(namedT("d"), namedT("e"))));
}

TEST(TupleTypeParsingTest, TupleTypeParsingNegativeTest)
{
    tuple_type_test_negative("(a&b)?");
    tuple_type_test_negative("(a|b)");
    tuple_type_test_negative("(a|b|c)");
    tuple_type_test_negative("(a&b)=>c");
    tuple_type_test_negative("(a)");
    tuple_type_test_negative("(a)?");
}
