#pragma once

#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
#include <lexer/TextArea.hpp>
#include <optional>

namespace lexing {
/**
 * @brief Enum class representing all the possible token types.
 *
 * This enum class includes all token types that a Lexer can recognize, including
 * various keywords, identifiers, operators, punctuation, and other language constructs.
 */
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

    // operators
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
    LT,
    LE,
    GT,
    GE,
    EQ,
    NEQ,
    DOT,

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
    COMMA,
    COLON,
    COLON_COLON,
    IDENTIFIER,
    TRUE,
    FALSE,
    SEMICOLON,
    END_OF_FILE
};

/**
 * @brief Returns a human-readable string description of a given TokenType.
 *
 * This function switches over all possible TokenTypes and returns a std::string_view
 * that describes the TokenType in a human-readable way.
 *
 * @param token The TokenType to describe.
 * @return A string_view describing the TokenType.
 */

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


/**
 * @brief Class representing a token in the source code.
 *
 * This class encapsulates a token in the source code, storing the type of the token,
 * the area in the source code the token occupies, and the value of the token.
 */
class Token
{

public:
    /**
     * @brief Constructs a new Token object.
     *
     * @param type The type of the token.
     * @param area The area in the source code that the token occupies.
     * @param value The value of the token.
     */
    constexpr Token(TokenTypes type, TextArea area,
                    std::string_view value) noexcept
        : type_(type), area_(area), value_(value) {}

    /**
     * @brief Constructs a new Token object from a type, start point, and value.
     *
     * @param type The type of the token.
     * @param start The starting point in the source code that the token occupies.
     * @param value The value of the token.
     */
    constexpr Token(TokenTypes type, std::uint64_t start,
                    std::string_view value) noexcept
        : type_(type),
          area_{start, start + value.size()},
          value_(value) {}

    /**
     * @brief Gets the type of the token.
     *
     * @return The type of the token.
     */
    constexpr auto getType() const noexcept -> TokenTypes
    {
        return type_;
    }
    /**
     * @brief Gets the area of the source code that the token occupies.
     *
     * @return The area of the source code that the token occupies.
     */
    constexpr auto getArea() const noexcept -> TextArea
    {
        return area_;
    }

    /**
     * @brief Gets the value of the token.
     *
     * @return The value of the token.
     */
    constexpr auto getValue() const noexcept -> std::string_view
    {
        return value_;
    }

    /**
     * @brief Checks if the token is a separator.
     *
     * A separator is a token that separates statements or expressions.
     * In this language, a semicolon and a newline are considered separators.
     *
     * @return true if the token is a separator, false otherwise.
     */
    constexpr auto isSeparator() const noexcept -> bool
    {
        return type_ == TokenTypes::SEMICOLON or type_ == TokenTypes::NEWLINE;
    }

    /**
     * @brief Checks if the token is an operator.
     *
     * An operator is a token that performs some operation on one or more operands.
     * This function checks whether the token is an infix, prefix or postfix operator.
     *
     * @return true if the token is an operator, false otherwise.
     */
    constexpr auto isOperator() const noexcept -> bool
    {
        return isInfixOperator() or isPostfixOperator() or isPrefixOperator();
    }

    /**
     * @brief Checks if the token is an infix operator.
     *
     * This function checks if the token's type is an infix operator. Infix operators are operators that
     * are written between the operands upon which they act. For example, in the expression "5 + 3", "+"
     * is an infix operator. In the context of this function, infix operators include the following token
     * types: PLUS, MINUS, ASTERIX, DIVISION, PERCENT, LOGICAL_OR, LOGICAL_AND, BITWISE_OR,
     * BITWISE_AND, LT, LE, GT, GE, EQ, NEQ, and DOT.
     *
     * @return bool Returns true if the token is an infix operator, false otherwise.
     */
    constexpr auto isInfixOperator() const noexcept -> bool
    {
        return type_ == TokenTypes::PLUS
            or type_ == TokenTypes::MINUS
            or type_ == TokenTypes::ASTERIX
            or type_ == TokenTypes::DIVISION
            or type_ == TokenTypes::PERCENT
            or type_ == TokenTypes::LOGICAL_OR
            or type_ == TokenTypes::LOGICAL_AND
            or type_ == TokenTypes::BITWISE_OR
            or type_ == TokenTypes::BITWISE_AND
            or type_ == TokenTypes::LT
            or type_ == TokenTypes::LE
            or type_ == TokenTypes::GT
            or type_ == TokenTypes::GE
            or type_ == TokenTypes::EQ
            or type_ == TokenTypes::NEQ
            or type_ == TokenTypes::DOT;
    }

    /**
     * @brief Checks if the token is a prefix operator.
     *
     * This function checks if the token's type is a prefix operator. Prefix operators are operators that
     * are written before their operands. For instance, in the expression "-5", "-" is a prefix operator.
     * In the context of this function, prefix operators include the following token types: PLUS, MINUS,
     * and LOGICAL_NOT.
     *
     * @return bool Returns true if the token is a prefix operator, false otherwise.
     */
    constexpr auto isPrefixOperator() const noexcept -> bool
    {
        return type_ == TokenTypes::PLUS
            or type_ == TokenTypes::MINUS
            or type_ == TokenTypes::LOGICAL_NOT;
    }

    /**
     * @brief Checks if the token is a postfix operator.
     *
     * This function checks if the token's type is a postfix operator. Postfix operators are operators that
     * are written after their operands. In this context, postfix operators include the following token types:
     * L_BRACKET, and L_PARANTHESIS. Function calls are postfix operations in Neon.
     *
     * @return bool Returns true if the token is a postfix operator, false otherwise.
     */
    constexpr auto isPostfixOperator() const noexcept -> bool
    {
        return type_ == TokenTypes::L_BRACKET
            or type_ == TokenTypes::L_PARANTHESIS;
    }


private:
    TokenTypes type_;
    TextArea area_;
    std::string_view value_;
};

} // namespace lexing
