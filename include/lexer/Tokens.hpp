#pragma once

#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
#include <lexer/TextArea.hpp>
#include <optional>

namespace lexing {

enum class TokenTypes : std::int64_t {
    LET = 0,
    FUN,
    INTEGER,
    STANDARD_STRING,
    DOUBLE,
    TYPE,
    TYPECLASS,
    WHITESPACE,
    NEWLINE,
    UNDERSCORE,
    QUESTIONMARK,
    LINE_COMMENT_START,
    L_PARANTHESIS,
    R_PARANTHESIS,
    L_BRACKET,
    R_BRACKET,
    PLUS,
    MINUS,
    ASTERIX,
    DIVISION,
    PERCENT,
    LOGICAL_OR,
    LOGICAL_AND,
    BITWISE_OR,
    BITWISE_AND,
    LOGICAL_NOT,
    ASSIGN,
    IF,
    ELSE,
    ELIF,
    WHILE,
    FOR,
    LAMBDA_ARROW,
    L_ARROW,
    R_ARROW,
    SELF_VALUE,
    SELF_TYPE,
    DOT,
    COMMA,
    COLON,
    COLON_COLON,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NEQ,
    IDENTIFIER,
    TRUE,
    FALSE,
    SEMICOLON,
    END_OF_FILE
};

constexpr auto get_description(TokenTypes token) noexcept -> std::string_view
{
    switch(token) {
    case TokenTypes::LET:
        return "LET";
    case TokenTypes::FUN:
        return "FUN";
    case TokenTypes::L_ARROW:
        return "L_ARROW";
    case TokenTypes::R_ARROW:
        return "R_ARROW";
    case TokenTypes::INTEGER:
        return "INTEGER";
    case TokenTypes::STANDARD_STRING:
        return "STANDARD_STRING";
    case TokenTypes::DOUBLE:
        return "DOUBLE";
    case TokenTypes::TYPE:
        return "TYPE";
    case TokenTypes::TYPECLASS:
        return "TYPECLASS";
    case TokenTypes::WHITESPACE:
        return "WHITESPACE";
    case TokenTypes::NEWLINE:
        return "NEWLINE";
    case TokenTypes::UNDERSCORE:
        return "UNDERSCORE";
    case TokenTypes::QUESTIONMARK:
        return "QUESTIONMARK";
    case TokenTypes::LINE_COMMENT_START:
        return "LINE_COMMENT_START";
    case TokenTypes::L_PARANTHESIS:
        return "L_PARANTHESIS";
    case TokenTypes::R_PARANTHESIS:
        return "R_PARANTHESIS";
    case TokenTypes::L_BRACKET:
        return "L_BRACKET";
    case TokenTypes::R_BRACKET:
        return "R_BRACKET";
    case TokenTypes::PLUS:
        return "PLUS";
    case TokenTypes::MINUS:
        return "MINUS";
    case TokenTypes::ASTERIX:
        return "ASTERIX";
    case TokenTypes::DIVISION:
        return "DIVISION";
    case TokenTypes::PERCENT:
        return "PERCENT";
    case TokenTypes::LOGICAL_OR:
        return "LOGICAL_OR";
    case TokenTypes::LOGICAL_AND:
        return "LOGICAL_AND";
    case TokenTypes::BITWISE_OR:
        return "BITWISE_OR";
    case TokenTypes::BITWISE_AND:
        return "BITWISE_AND";
    case TokenTypes::LOGICAL_NOT:
        return "LOGICAL_NOT";
    case TokenTypes::ASSIGN:
        return "ASSIGN";
    case TokenTypes::IF:
        return "IF";
    case TokenTypes::ELIF:
        return "ELIF";
    case TokenTypes::ELSE:
        return "ELSE";
    case TokenTypes::WHILE:
        return "WHILE";
    case TokenTypes::FOR:
        return "FOR";
    case TokenTypes::LAMBDA_ARROW:
        return "LAMBDA_ARROW";
    case TokenTypes::SELF_VALUE:
        return "SELF_VALUE";
    case TokenTypes::SELF_TYPE:
        return "SELF_TYPE";
    case TokenTypes::DOT:
        return "DOT";
    case TokenTypes::COLON:
        return "COLON";
    case TokenTypes::COMMA:
        return "COMMA";
    case TokenTypes::LT:
        return "LT";
    case TokenTypes::LE:
        return "LE";
    case TokenTypes::GT:
        return "GT";
    case TokenTypes::GE:
        return "GE";
    case TokenTypes::EQ:
        return "EQ";
    case TokenTypes::NEQ:
        return "NEQ";
    case TokenTypes::IDENTIFIER:
        return "IDENTIFIER";
    case TokenTypes::TRUE:
        return "TRUE";
    case TokenTypes::FALSE:
        return "FALSE";
    case TokenTypes::COLON_COLON:
        return "COLON_COLON";
    case TokenTypes::SEMICOLON:
        return "SEMICOLON";
    case TokenTypes::END_OF_FILE:
        return "<EOF>";
    default:
        return "<UNKNOWN TOKEN>";
    }
}



class Token
{

public:
    constexpr Token(TokenTypes type, TextArea area,
                    std::string_view value) noexcept
        : type_(type), area_(area), value_(value) {}

    constexpr Token(TokenTypes type, std::uint64_t start,
                    std::string_view value) noexcept
        : type_(type),
          area_{start, start + value.size()},
          value_(value) {}

    constexpr auto getType() const noexcept -> TokenTypes
    {
        return type_;
    }
    constexpr auto getArea() const noexcept -> TextArea
    {
        return area_;
    }
    constexpr auto getValue() const noexcept -> std::string_view
    {
        return value_;
    }

    constexpr auto isSeparator() const noexcept -> bool
    {
        return type_ == TokenTypes::SEMICOLON or type_ == TokenTypes::NEWLINE;
    }

private:
    TokenTypes type_;
    TextArea area_;
    std::string_view value_;
};

} // namespace lexing
