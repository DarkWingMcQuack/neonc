#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class ForExprLetElement : public AreaBase
{
    ForExprLetElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
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

class ForExprMonadicElement : public AreaBase
{
    ForExprMonadicElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
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

using ForExprElement = std::variant<ForExprLetElement, ForExprMonadicElement>;

class ForExpr : public AreaBase
{
public:
    ForExpr(lexing::TextArea area, std::vector<ForExprElement>&& elements, Expression&& ret_expr) noexcept
        : AreaBase(area),
          elements_(std::move(elements)),
          ret_expr_(std::move(ret_expr)) {}

    auto getElements() const noexcept -> const std::vector<ForExprElement>&
    {
        return elements_;
    }
    auto getElements() noexcept -> std::vector<ForExprElement>&
    {
        return elements_;
    }

    auto getReturnExpression() const noexcept -> const Expression&
    {
        return ret_expr_;
    }
    auto getReturnExpression() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    std::vector<ForExprElement> elements_;
    Expression ret_expr_;
};

} // namespace ast
