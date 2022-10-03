#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

inline auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

inline auto if_(auto condition, auto then, auto else_) -> ast::Expression
{
    std::vector<ast::ElifExpr> v;
    return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
                                     std::move(condition),
                                     std::move(then),
                                     std::move(v),
                                     std::move(else_));
}

inline auto let(auto name, auto rhs) -> ast::Statement
{
    return ast::forward<ast::LetAssignment>(lexing::TextArea{0, 0},
                                            std::move(name),
                                            std::nullopt,
                                            std::move(rhs));
}

inline auto let(auto name, auto type, auto rhs) -> ast::Statement
{
    return ast::forward<ast::LetAssignment>(lexing::TextArea{0, 0},
                                            std::move(name),
                                            std::move(type),
                                            std::move(rhs));
}

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

auto let_test_positive(std::string_view text, auto expected)
{
    auto result = Parser{text}.statement();

    ASSERT_TRUE(!!result);
    ASSERT_TRUE(std::holds_alternative<ast::Forward<ast::LetAssignment>>(result.value()));

    EXPECT_EQ((result.value()), expected);
}

auto let_test_negative(std::string_view text)
{
    auto result = Parser{text}.statement();

    ASSERT_TRUE(!!result);
    EXPECT_FALSE(std::holds_alternative<ast::Forward<ast::LetAssignment>>(result.value()));
}

TEST(LetStatementParserTest, LetStatementParsingWithoutTypePositiveTest)
{
    let_test_positive("let a = b", let(id("a"), id("b")));
    let_test_positive("let a = if(a) b else c", let(id("a"), if_(id("a"), id("b"), id("c"))));
    let_test_positive("let aasd = if(a) b else c", let(id("aasd"), if_(id("a"), id("b"), id("c"))));
    let_test_positive("let _aasd3 = if(a) b else c", let(id("_aasd3"), if_(id("a"), id("b"), id("c"))));
    let_test_positive("let __aasd3_ = if(a) b else c", let(id("__aasd3_"), if_(id("a"), id("b"), id("c"))));
    // clang-format off
	let_test_positive("let a = if(a) if(b) c else d else c", let(id("a"),
																 if_(id("a"),
																	 if_(id("b"),
																		 id("c"),
																		 id("d")),
																	 id("c"))));

	let_test_positive("let b = if(a) if(b) c else d else if(x) y else z", let(id("b"),
																			  if_(id("a"),
																				  if_(id("b"),
																					  id("c"),
																					  id("d")),
																				  if_(id("x"),
																					  id("y"),
																					  id("z")))));
    // clang-format on
}

TEST(LetStatementParserTest, LetStatementParsingWithTypePositiveTest)
{
    let_test_positive("let a: Int = b", let(id("a"), namedT("Int"), id("b")));

    let_test_positive("let a: Int => Int = if(a) b else c",
                      let(id("a"),
                          lambdaT(namedT("Int"), namedT("Int")),
                          if_(id("a"), id("b"), id("c"))));

    let_test_positive("let a: (Int | Double) = b",
                      let(id("a"),
                          unionT(namedT("Int"), namedT("Double")),
                          id("b")));

    let_test_positive("let a: (Int | Double) => String = b",
                      let(id("a"),
                          lambdaT(unionT(namedT("Int"), namedT("Double")),
                                  namedT("String")),
                          id("b")));

    let_test_positive("let aasd : (Int & Double & String) = if(a) b else c",
                      let(id("aasd"),
                          tupleT(namedT("Int"), namedT("Double"), namedT("String")),
                          if_(id("a"), id("b"), id("c"))));
}

TEST(LetStatementParserTest, LetStatementiParsingNegativeTest)
{
    let_test_negative("hello=>a");
    let_test_negative("(hello,hello)");
    let_test_negative("self");
}
