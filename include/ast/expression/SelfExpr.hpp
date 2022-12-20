#pragma once

#include <ast/common/AreaBase.hpp>
#include <utility>

namespace ast {

class SelfExpr : public AreaBase
{
public:
    constexpr SelfExpr(lexing::TextArea area) noexcept
        : AreaBase{std::move(area)} {}

    constexpr SelfExpr() noexcept = delete;
    constexpr SelfExpr(SelfExpr&&) noexcept = default;
    constexpr SelfExpr(const SelfExpr&) noexcept = default;
    auto operator=(SelfExpr&&) noexcept -> SelfExpr& = default;
    auto operator=(const SelfExpr&) noexcept -> SelfExpr& = default;

    constexpr auto operator==(const SelfExpr&) const noexcept
        -> bool
    {
        return true;
    }
    constexpr auto operator!=(const SelfExpr&) const noexcept
        -> bool
    {
        return false;
    }
};

} // namespace ast
