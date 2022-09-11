#pragma once

#include <ast/Forward.hpp>
#include <common/Traits.hpp>
#include <variant>

namespace ast::utils {
template<class Visitor, class Variant>
auto visit(Visitor&& visitor, Variant&& var) noexcept
{
    if constexpr(not common::is_specialization_of<std::variant, Variant>::value) {
        if constexpr(std::is_void_v<std::decay_t<decltype(visitor(var))>>) {
            std::forward<Visitor>(visitor)(std::forward<Variant>(var));
        } else {
            return std::forward<Visitor>(visitor)(std::forward<Variant>(var));
        }
    } else {
        if constexpr(std::is_void_v<std::decay_t<decltype(std::visit(visitor, var))>>) {
            std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(var));
        } else {
            return std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(var));
        }
    }
}

} // namespace ast::utils
