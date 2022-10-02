#include <iostream>
#include <lexer/Lexer.hpp>

#include <gtest/gtest.h>


using lexing::Lexer;

auto assertStringToLexedToken(const std::string& input,
                              lexing::TokenTypes expectedToken) -> void
{
    auto lexer = lexing::Lexer(input);
    auto actualToken = lexer.peek();
    if(!actualToken.has_value())
        FAIL() << "actual token is empty; expected token: "
               << lexing::get_description(expectedToken) << " for input string: \""
               << input << "\"";
    ASSERT_EQ(actualToken.value().getType(), expectedToken)
        << "input string: \"" << input
        << "\" did not yield expected lexer token: "
        << lexing::get_description(expectedToken) << "; returned token is: "
        << lexing::get_description(actualToken.value().getType());
}

TEST(LexerTest, SingleTokenTest)
{
    assertStringToLexedToken("let", lexing::TokenTypes::LET);
    assertStringToLexedToken("fun", lexing::TokenTypes::FUN);
    assertStringToLexedToken("_", lexing::TokenTypes::UNDERSCORE);
    assertStringToLexedToken("?", lexing::TokenTypes::QUESTIONMARK);
    assertStringToLexedToken("::", lexing::TokenTypes::COLON_COLON);
    assertStringToLexedToken("42", lexing::TokenTypes::INTEGER);
    assertStringToLexedToken("\"i am a string\"", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken("3.1415", lexing::TokenTypes::DOUBLE);
    assertStringToLexedToken("3.1415e19", lexing::TokenTypes::DOUBLE);
    assertStringToLexedToken("type", lexing::TokenTypes::TYPE);
    assertStringToLexedToken("typeclass", lexing::TokenTypes::TYPECLASS);
    assertStringToLexedToken("\n", lexing::TokenTypes::NEWLINE);
    assertStringToLexedToken("\n\n\n\n", lexing::TokenTypes::NEWLINE);
    assertStringToLexedToken("//", lexing::TokenTypes::LINE_COMMENT_START);
    assertStringToLexedToken("(", lexing::TokenTypes::L_PARANTHESIS);
    assertStringToLexedToken(")", lexing::TokenTypes::R_PARANTHESIS);
    assertStringToLexedToken("{", lexing::TokenTypes::L_BRACKET);
    assertStringToLexedToken("}", lexing::TokenTypes::R_BRACKET);
    assertStringToLexedToken("+", lexing::TokenTypes::PLUS);
    assertStringToLexedToken("-", lexing::TokenTypes::MINUS);
    assertStringToLexedToken("*", lexing::TokenTypes::ASTERIX);
    assertStringToLexedToken("/", lexing::TokenTypes::DIVISION);
    assertStringToLexedToken("%", lexing::TokenTypes::PERCENT);
    assertStringToLexedToken("||", lexing::TokenTypes::LOGICAL_OR);
    assertStringToLexedToken("&&", lexing::TokenTypes::LOGICAL_AND);
    assertStringToLexedToken("|", lexing::TokenTypes::BITWISE_OR);
    assertStringToLexedToken("&", lexing::TokenTypes::BITWISE_AND);
    assertStringToLexedToken("!", lexing::TokenTypes::LOGICAL_NOT);
    assertStringToLexedToken("=", lexing::TokenTypes::ASSIGN);
    assertStringToLexedToken("if", lexing::TokenTypes::IF);
    assertStringToLexedToken("else", lexing::TokenTypes::ELSE);
    assertStringToLexedToken("elif", lexing::TokenTypes::ELIF);
    assertStringToLexedToken("while", lexing::TokenTypes::WHILE);
    assertStringToLexedToken("for", lexing::TokenTypes::FOR);
    assertStringToLexedToken("=>", lexing::TokenTypes::LAMBDA_ARROW);
    assertStringToLexedToken("<-", lexing::TokenTypes::L_ARROW);
    assertStringToLexedToken("->", lexing::TokenTypes::R_ARROW);
    assertStringToLexedToken("self", lexing::TokenTypes::SELF_VALUE);
    assertStringToLexedToken("Self", lexing::TokenTypes::SELF_TYPE);
    assertStringToLexedToken(".", lexing::TokenTypes::DOT);
    assertStringToLexedToken(",", lexing::TokenTypes::COMMA);
    assertStringToLexedToken(":", lexing::TokenTypes::COLON);
    assertStringToLexedToken("<", lexing::TokenTypes::LT);
    assertStringToLexedToken("<=", lexing::TokenTypes::LE);
    assertStringToLexedToken(">", lexing::TokenTypes::GT);
    assertStringToLexedToken(">=", lexing::TokenTypes::GE);
    assertStringToLexedToken("==", lexing::TokenTypes::EQ);
    assertStringToLexedToken("!=", lexing::TokenTypes::NEQ);
    assertStringToLexedToken("x", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("true", lexing::TokenTypes::TRUE);
    assertStringToLexedToken("false", lexing::TokenTypes::FALSE);
    assertStringToLexedToken("", lexing::TokenTypes::END_OF_FILE);
}


TEST(LexerTest, IdentifierTest)
{
    assertStringToLexedToken("_let", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("afun", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("asdf", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("d", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("letsda", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("true1", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("true_", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken("_true_", lexing::TokenTypes::IDENTIFIER);
}

TEST(LexerTest, WhitespaceSkipTest)
{
    assertStringToLexedToken(" _let", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken(" \t\t  _let", lexing::TokenTypes::IDENTIFIER);
    assertStringToLexedToken(" 2", lexing::TokenTypes::INTEGER);
    assertStringToLexedToken(" \t   2", lexing::TokenTypes::INTEGER);
}

TEST(LexerTest, TrippleTokenTest)
{
    auto lexer = lexing::Lexer{"let fun let"};
    auto result = lexer.peek<3>();

    ASSERT_TRUE(!!result);

    auto [l, f, l2] = result.value();

    EXPECT_EQ(l.getType(), lexing::TokenTypes::LET);
    EXPECT_EQ(f.getType(), lexing::TokenTypes::FUN);
    EXPECT_EQ(l2.getType(), lexing::TokenTypes::LET);

    result = lexer.peek<3>();

    ASSERT_TRUE(!!result);

    auto [l3, f2, l4] = result.value();

    EXPECT_EQ(l3.getType(), lexing::TokenTypes::LET);
    EXPECT_EQ(f2.getType(), lexing::TokenTypes::FUN);
    EXPECT_EQ(l4.getType(), lexing::TokenTypes::LET);

    lexer.pop<3>();
    auto r_end = lexer.peek();
    ASSERT_TRUE(!!r_end);

    EXPECT_EQ(r_end.value().getType(), lexing::TokenTypes::END_OF_FILE);
}

auto nextShouldBe(Lexer& lexer, lexing::TokenTypes expectedToken)
{
    auto actualToken = lexer.peek();

    // clang-format off
    while(actualToken.has_value() and
		  (actualToken.value().getType() == lexing::TokenTypes::WHITESPACE
		   or actualToken.value().getType() == lexing::TokenTypes::NEWLINE)) {
        lexer.pop();
        actualToken = lexer.peek();
    }
    // clang-format on

    if(!actualToken.has_value())
        FAIL() << "actual token is empty; expected token: "
               << lexing::get_description(expectedToken);

    lexer.pop();

    ASSERT_EQ(actualToken.value().getType(), expectedToken)
        << "input string: \"" << actualToken.value().getValue()
        << "\" did not yield expected lexer token: "
        << lexing::get_description(expectedToken) << "; returned token is: "
        << lexing::get_description(actualToken.value().getType());
}

TEST(LexerTest, FunctionDeclarationTest)
{
    auto input = R"(
        fun foo(int a, int b)
        {
            let a = 5
        }
    )";

    auto lexer = lexing::Lexer(input);

    nextShouldBe(lexer, lexing::TokenTypes::FUN);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::L_PARANTHESIS);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::COMMA);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::R_PARANTHESIS);
    nextShouldBe(lexer, lexing::TokenTypes::L_BRACKET);
    nextShouldBe(lexer, lexing::TokenTypes::LET);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::ASSIGN);
    nextShouldBe(lexer, lexing::TokenTypes::INTEGER);
    nextShouldBe(lexer, lexing::TokenTypes::R_BRACKET);
    nextShouldBe(lexer, lexing::TokenTypes::END_OF_FILE);
}


TEST(LexerTest, StringLexTest)
{
    assertStringToLexedToken("\"asddas tis is a string\"", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken("\"asddas \\\"tis is a string, to\"", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken(R"("this is a string yes")", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken(R"("this \"is a string yes")", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken(R"("this \\is a string yes")", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken(R"("this \n is a string yes")", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken(R"("this \t  is a string yes")", lexing::TokenTypes::STANDARD_STRING);
}

TEST(LexerTest, LineCommentTest)
{
    auto input = R"(
               //asdasdsdasd
               asdsd
               )";

    auto lexer = lexing::Lexer(input);

    nextShouldBe(lexer, lexing::TokenTypes::LINE_COMMENT_START);
    nextShouldBe(lexer, lexing::TokenTypes::IDENTIFIER);
    nextShouldBe(lexer, lexing::TokenTypes::END_OF_FILE);
}
