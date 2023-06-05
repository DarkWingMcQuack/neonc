#pragma once

#include <ctre/ctre.hpp>

namespace lexing {
// whitespace and newline
constinit auto ws_re = ctre::starts_with<"[ \f\r\t\v]+">;
constinit auto nl_re = ctre::starts_with<"\\n+">;

// keywords
constinit auto let_re = ctre::starts_with<"let([^a-zA-Z0-9_]|$)">;
constinit auto underscore_re = ctre::starts_with<"_([^a-zA-Z0-9_]|$)">;
constinit auto fun_re = ctre::starts_with<"fun([^a-zA-Z0-9_]|$)">;
constinit auto type_re = ctre::starts_with<"type([^a-zA-Z0-9_]|$)">;
constinit auto typeclass_re = ctre::starts_with<"typeclass([^a-zA-Z0-9_]|$)">;
constinit auto if_re = ctre::starts_with<"if([^a-zA-Z0-9_]|$)">;
constinit auto elif_re = ctre::starts_with<"elif([^a-zA-Z0-9_]|$)">;
constinit auto else_re = ctre::starts_with<"else([^a-zA-Z0-9_]|$)">;
constinit auto while_re = ctre::starts_with<"while([^a-zA-Z0-9_]|$)">;
constinit auto for_re = ctre::starts_with<"for([^a-zA-Z0-9_]|$)">;
constinit auto self_value_re = ctre::starts_with<"self([^a-zA-Z0-9_]|$)">;
constinit auto self_type_re = ctre::starts_with<"Self([^a-zA-Z0-9_]|$)">;
constinit auto true_re = ctre::starts_with<"true([^a-zA-Z0-9_]|$)">;
constinit auto false_re = ctre::starts_with<"false([^a-zA-Z0-9_]|$)">;

// values
constinit auto integer_re = ctre::starts_with<"0|[1-9][0-9]*">;
constinit auto identifier_re = ctre::starts_with<"[a-zA-Z_][a-zA-Z_0-9]*">;
constinit auto double_re = ctre::starts_with<"((\\d+\\.\\d*)|(\\d*\\.\\d+))([eE][+\\-]?\\d+)?">;

} // namespace lexing
