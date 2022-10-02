#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <concepts>
#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

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

inline auto union_type_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<ast::Forward<ast::UnionType>>(result.value()));
    EXPECT_EQ(result.value(), expected);
}

inline auto union_type_test_negative(std::string_view text)
{
    auto result = Parser{text}.type();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::UnionType>>(result.value()));
}


TEST(TupleTypeParsingTest, TupleTypeParsingPositiveTest)
{
    union_type_test_positive("(a|b)", unionT(namedT("a"), namedT("b")));
    union_type_test_positive("(a|b|c)", unionT(namedT("a"), namedT("b"), namedT("c")));
    union_type_test_positive("(a|b|c|d)", unionT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    union_type_test_positive("((a|b|c|d))", unionT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    union_type_test_positive("(((a|b|c|d)))", unionT(namedT("a"), namedT("b"), namedT("c"), namedT("d")));
    union_type_test_positive("((a|b|c|(d=>e)))", unionT(namedT("a"), namedT("b"), namedT("c"), lambdaT(namedT("d"), namedT("e"))));
}

TEST(UnionTypeParsingTest, UnionTypeParsingNegativeTest)
{
    union_type_test_negative("(a&b)?");
    union_type_test_negative("(a|b)?");
    union_type_test_negative("(a&b)");
    union_type_test_negative("(a&b&c)");
    union_type_test_negative("(a&b)=>c");
    union_type_test_negative("(a)");
    union_type_test_negative("(a)?");
}
