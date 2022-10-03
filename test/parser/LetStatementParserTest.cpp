#include <iostream>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

#include <gtest/gtest.h>

using parser::Parser;

auto id(std::string_view text) -> ast::Identifier
{
    return ast::Identifier{{0, 0}, text};
}

auto if_(auto condition, auto then, auto else_) -> ast::Expression
{
    std::vector<ast::ElifExpr> v;
    return ast::forward<ast::IfExpr>(lexing::TextArea{0, 0},
                                     std::move(condition),
                                     std::move(then),
                                     std::move(v),
                                     std::move(else_));
}

auto let(auto name, auto rhs) -> ast::Statement
{
    return ast::forward<ast::LetAssignment>(lexing::TextArea{0, 0},
                                            std::move(name),
                                            std::nullopt,
                                            std::move(rhs));
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

TEST(LetStatementParserTest, LetStatementiParsingNegativeTest)
{
    let_test_negative("hello=>a");
    let_test_negative("(hello,hello)");
    let_test_negative("self");
}
