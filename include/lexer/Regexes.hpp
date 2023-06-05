#pragma once

#include <ctre/ctre.hpp>

namespace lexing {

/// Regular expressions using compile-time regular expressions (CTRE) library to match tokens.

/// @brief Matches whitespace characters at the start of a string.
/// @details This regex matches one or more of the following whitespace characters: space, form feed, carriage return, tab, and vertical tab.
constinit auto ws_re = ctre::starts_with<"[ \f\r\t\v]+">;

/// @brief Matches newline characters at the start of a string.
/// @details This regex matches one or more newline characters ('\n').
constinit auto nl_re = ctre::starts_with<"\\n+">;

/// Keyword regular expressions:

/// @brief Matches the 'let' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto let_re = ctre::starts_with<"let([^a-zA-Z0-9_]|$)">;

/// @brief Matches the '_' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto underscore_re = ctre::starts_with<"_([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'fun' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto fun_re = ctre::starts_with<"fun([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'type' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto type_re = ctre::starts_with<"type([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'typeclass' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto typeclass_re = ctre::starts_with<"typeclass([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'if' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto if_re = ctre::starts_with<"if([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'elif' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto elif_re = ctre::starts_with<"elif([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'else' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto else_re = ctre::starts_with<"else([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'while' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto while_re = ctre::starts_with<"while([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'for' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto for_re = ctre::starts_with<"for([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'self' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto self_value_re = ctre::starts_with<"self([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'Self' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto self_type_re = ctre::starts_with<"Self([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'true' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto true_re = ctre::starts_with<"true([^a-zA-Z0-9_]|$)">;

/// @brief Matches the 'false' keyword at the start of a string.
/// @details The keyword must not be followed directly by a letter, digit, or underscore.
constinit auto false_re = ctre::starts_with<"false([^a-zA-Z0-9_]|$)">;

/// Value regular expressions:

/// @brief Matches an integer at the start of a string.
/// @details This regex matches a zero or a non-zero integer that does not start with a zero.
constinit auto integer_re = ctre::starts_with<"0|[1-9][0-9]*">;

/// @brief Matches an identifier at the start of a string.
/// @details This regex matches a string that starts with a letter or an underscore, followed by any number of letters, digits, or underscores.
constinit auto identifier_re = ctre::starts_with<"[a-zA-Z_][a-zA-Z_0-9]*">;

/// @brief Matches a double at the start of a string.
/// @details This regex matches a string that represents a floating-point number.
///          This can include an integer part, a decimal part, and an optional exponent part.
constinit auto double_re = ctre::starts_with<"((\\d+\\.\\d*)|(\\d*\\.\\d+))([eE][+\\-]?\\d+)?">;




} // namespace lexing
