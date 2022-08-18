#pragma once

#include <ast/Ast.hpp>
#include <memory>

namespace ast {

class Remainder : public AreaBase
{
public:
    Remainder(lexing::TextArea area, Expression&& lhs, Expression&& rhs) noexcept
        : AreaBase(area),
          lhs_(std::move(lhs)),
          rhs_(std::move(rhs)) {}

    auto getLeftHandSide() const noexcept -> const Expression&
    {
        return lhs_;
    }
    auto getLeftHandSide() noexcept -> Expression&
    {
        return lhs_;
    }

    auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    auto getightSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Expression lhs_;
    Expression rhs_;
};

} // namespace ast
