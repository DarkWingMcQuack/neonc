#pragma once

#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
#include <fmt/core.h>
#include <lexer/Regexes.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <optional>
#include <queue>
#include <string_view>
#include <vector>

namespace common {

// clang-format off
template < template <typename...> class Template, typename T >
struct is_specialization_of : std::false_type {};

template < template <typename...> class Template, typename... Args >
struct is_specialization_of< Template, Template<Args...> > : std::true_type {};
// clang-format on

} // namespace common
