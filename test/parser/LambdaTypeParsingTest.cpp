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

inline auto namedT(auto... elems) -> ast::NamedType
{
    static_assert(sizeof...(elems) > 0);

    auto ns = std::vector<ast::Identifier>{ast::Identifier{{0, 0}, elems}...};
    auto last = ns.back();
    ns.pop_back();

    return ast::NamedType{{0, 0}, std::move(ns), std::move(last)};
}

inline auto selfT() -> ast::SelfType
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

inline auto lambda_type_test_positive(std::string_view text, auto expected)
{
    lexing::Lexer lexer{text};
    auto result = TypeParser{text, std::move(lexer)}.type();

    ASSERT_TRUE(!!result);
    EXPECT_TRUE(std::holds_alternative<ast::Forward<ast::LambdaType>>(result.value()));
    EXPECT_EQ(*std::get<ast::Forward<ast::LambdaType>>(result.value()),
              *std::get<ast::Forward<ast::LambdaType>>(expected));
}

inline auto lambda_type_test_negative(std::string_view text)
{
    lexing::Lexer lexer{text};
    auto result = TypeParser{text, std::move(lexer)}.type();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::LambdaType>>(result.value()));
}


TEST(LambdaTypeParsingTest, OptionalTypeParsingPositiveTest)
{
    lambda_type_test_positive("hello=>hello", lambdaT(namedT("hello"), namedT("hello")));
}

TEST(LambdaTypeParsingTest, OptionalTypeParsingNegativeTest)
{
    lambda_type_test_negative("hello");
}
