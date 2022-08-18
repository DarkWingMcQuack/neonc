#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class LetAssignment : public AreaBase
{
    LetAssignment(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_;
    Expression rhs_;
};

} // namespace ast
