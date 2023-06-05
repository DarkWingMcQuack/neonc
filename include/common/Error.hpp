#pragma once

#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <source_location>
#include <variant>
#include <vector>

namespace common::error {


/**
 * @brief Represents an unknown token error.
 *
 * This error class is used when a token is encountered in the source code that the lexer does not recognize.
 * An instance of this class holds information about the location of the unknown token in the source code.
 */
class UnknownToken
{
public:
    /**
     * @brief Constructs an UnknownToken error object.
     *
     * @param area The area in the source code where the unknown token is located.
     */
    constexpr UnknownToken(lexing::TextArea area) noexcept
        : area_(std::move(area)) {}

private:
    lexing::TextArea area_;
};

/**
 * @brief Represents an unclosed string error.
 *
 * This error class is used when an unclosed string is found in the source code. An instance of this class
 * holds information about the location of the unclosed string in the source code.
 */
class UnclosedString
{
public:
    /**
     * @brief Constructs an UnclosedString error object.
     *
     * @param area The area in the source code where the unclosed string starts.
     */
    constexpr UnclosedString(lexing::TextArea area) noexcept
        : area_(std::move(area)) {}

private:
    lexing::TextArea area_;
};

/**
 * @brief Represents an unexpected token error.
 *
 * This error class is used when a token is found in the source code where it was not expected. An instance of
 * this class holds information about the actual token encountered and the expected tokens.
 */
class UnexpectedToken
{
public:
    /**
     * @brief Constructs an UnexpectedToken error object.
     *
     * @param actual The actual token that was found in the source code.
     * @param area The area in the source code where the unexpected token is located.
     * @param expected The expected tokens.
     */
    template<class... Exp>
        requires(sizeof...(Exp) >= 1) and (std::same_as<Exp, lexing::TokenTypes> and ...)
    constexpr UnexpectedToken(lexing::TokenTypes actual,
                              lexing::TextArea area,
                              Exp... expected) noexcept
        : area_(std::move(area)),
          actual_(std::move(actual)),
          expected_{std::move(expected)...}
    {}

private:
    lexing::TextArea area_;
    lexing::TokenTypes actual_;
    std::vector<lexing::TokenTypes> expected_;
};

/**
 * @brief Represents a lambda annotations in parameter not enclosed error.
 *
 * This error class is used when lambda annotations in a parameter are not properly enclosed. An instance of this
 * class holds information about the area in the source code where this error occurred.
 */
class LambdaAnotationsInParameterNotEnclosed
{
public:
    /**
     * @brief Constructs a LambdaAnotationsInParameterNotEnclosed error object.
     *
     * @param area The area in the source code where the error is located.
     */
    constexpr LambdaAnotationsInParameterNotEnclosed(lexing::TextArea area) noexcept
        : area_(std::move(area)) {}

private:
    lexing::TextArea area_;
};

/**
 * @brief Represents an internal compiler error.
 *
 * This error class is used to represent errors that are not caused by the source code being compiled, but by
 * an error in the compiler itself. An instance of this class holds information about the location in the
 * compiler source code where the error occurred.
 */
class InternalCompilerError
{
public:
    /**
     * @brief Constructs an InternalCompilerError object.
     *
     * @param location The location in the compiler source code where the error occurred.
     * Defaults to the location where this function was called.
     */
    constexpr InternalCompilerError(std::source_location location = std::source_location::current()) noexcept
        : location_(std::move(location)) {}

private:
    std::source_location location_;
};

/**
 * @brief Variant type that can hold any of the error types defined above.
 */
using Error = std::variant<UnknownToken,
                           UnclosedString,
                           UnexpectedToken,
                           LambdaAnotationsInParameterNotEnclosed,
                           InternalCompilerError>;
} // namespace common::error
