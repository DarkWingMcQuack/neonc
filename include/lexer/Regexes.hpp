#pragma once

#include <ctre/ctre.hpp>

namespace lexing {
// whitespace and newline
constexpr auto ws_re = ctre::starts_with<"\\s+">;
constexpr auto nl_re = ctre::starts_with<"\\n+">;

// keywords
constexpr auto let_re = ctre::starts_with<"let\\s">;
constexpr auto fun_re = ctre::starts_with<"fun\\s">;
constexpr auto type_re = ctre::starts_with<"type\\s">;
constexpr auto typeclass_re = ctre::starts_with<"typeclass\\s">;
constexpr auto if_re = ctre::starts_with<"if[^a-zA-Z0-9_]">;
constexpr auto elif_re = ctre::starts_with<"elif[^a-zA-Z0-9_]">;
constexpr auto else_re = ctre::starts_with<"else[^a-zA-Z0-9_]">;
constexpr auto while_re = ctre::starts_with<"while[^a-zA-Z0-9_]">;
constexpr auto for_re = ctre::starts_with<"for[^a-zA-Z0-9_]">;
constexpr auto self_value_re = ctre::starts_with<"self[^a-zA-Z0-9_]">;
constexpr auto self_type_re = ctre::starts_with<"Self[^a-zA-Z0-9_]">;
constexpr auto true_re = ctre::starts_with<"true\\s">;
constexpr auto false_re = ctre::starts_with<"false\\s">;

// values
constexpr auto integer_re = ctre::starts_with<"0|[1-9][0-9]*">;
constexpr auto identifier_re = ctre::starts_with<"[a-zA-Z_][a-zA-Z_0-9]*">;
constexpr auto double_re = ctre::starts_with<"[0-9]+\\.[0-9]*([eE][+\\-]?[0-9]+)?">;

} // namespace lexing
