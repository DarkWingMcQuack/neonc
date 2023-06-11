#pragma once

#include <ast/Forward.hpp>
#include <common/Traits.hpp>
#include <variant>

namespace ast::utils {
/**
 * @brief Visits a variant object with a visitor.
 * @details This function template provides a generic way to visit a variant object with a visitor function or lambda.
 * It uses compile-time type checks to handle both std::variant and non-std::variant types.
 * @tparam Visitor The type of the visitor function or lambda.
 * @tparam Variant The type of the variant object.
 * @param visitor The visitor function or lambda to be invoked for each alternative in the variant.
 * @param var The variant object to be visited.
 * @return The result of invoking the visitor function or lambda, if any.
 * @note The visitor function or lambda should have overloads for each alternative in the variant, or a default fallback overload.
 * If the visitor returns void, the function performs a void visit and does not return any result.
 * If the visitor returns a non-void type, the function returns the result of the visitor invocation.
 */
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
