#include <iostream>
#include <lexer/Lexer.hpp>

#include <gtest/gtest.h>


auto assertStringToLexedToken(const std::string& input, lexing::TokenTypes expectedToken) -> void
{
    auto lexer = lexing::Lexer(input);
    auto actualToken = lexer.peek();
    if(!actualToken.has_value())
        FAIL() << "actual token is empty; expected token: "
               << lexing::get_description(expectedToken)
               << " for input string: \"" << input << "\"";
    ASSERT_EQ(actualToken.value().getType(), expectedToken)
        << "input string: \"" << input << "\" did not yield expected lexer token: "
        << lexing::get_description(expectedToken) << "; returned token is: "
        << lexing::get_description(actualToken.value().getType());
}

TEST(LexerTest, SingleTokenTest)
{
    assertStringToLexedToken("let", lexing::TokenTypes::LET);
    assertStringToLexedToken("fun", lexing::TokenTypes::FUN);
    assertStringToLexedToken("42", lexing::TokenTypes::INTEGER);
    assertStringToLexedToken("\"i am a string\"", lexing::TokenTypes::STANDARD_STRING);
    assertStringToLexedToken("3.1415 ", lexing::TokenTypes::DOUBLE);
    assertStringToLexedToken("type", lexing::TokenTypes::TYPE);
    assertStringToLexedToken("typeclass", lexing::TokenTypes::TYPECLASS);
    assertStringToLexedToken(" ", lexing::TokenTypes::WHITESPACE);
    assertStringToLexedToken("\n", lexing::TokenTypes::NEWLINE);
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

TEST(LexerTest, DoubleTokenTest)
{
    auto lexer = lexing::Lexer{"let fun"};
    auto result = lexer.peek<3>();

    ASSERT_TRUE(!!result);

    auto [l, w, f] = result.value();

    EXPECT_EQ(l.getType(), lexing::TokenTypes::LET);
    EXPECT_EQ(w.getType(), lexing::TokenTypes::WHITESPACE);
    EXPECT_EQ(f.getType(), lexing::TokenTypes::FUN);

    result = lexer.peek<3>();

    ASSERT_TRUE(!!result);

    auto [l2, w2, f2] = result.value();

    EXPECT_EQ(l2.getType(), lexing::TokenTypes::LET);
    EXPECT_EQ(w2.getType(), lexing::TokenTypes::WHITESPACE);
    EXPECT_EQ(f2.getType(), lexing::TokenTypes::FUN);
}

TEST(LexerTest, FunctionDeclarationTest)
{
    std::string input = R"(
        fun foo(int a, int b)
        {
            let a = 5
        }
    )";
    auto lexer = lexing::Lexer(input);

    while(auto token = lexer.peek()) {
        std::cout << lexing::get_description(token.value().getType()) << "\n";
        if(token.value().getType() == lexing::TokenTypes::END_OF_FILE) {
            break;
        }
        lexer.pop();
    }
}
