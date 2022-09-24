#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class ForExprLetElement : public AreaBase
{
    constexpr ForExprLetElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_;
    Expression rhs_;
};

class ForExprMonadicElement : public AreaBase
{
    constexpr ForExprMonadicElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
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
    constexpr ForExpr(lexing::TextArea area, std::vector<ForExprElement>&& elements, Expression&& ret_expr) noexcept
        : AreaBase(area),
          elements_(std::move(elements)),
          ret_expr_(std::move(ret_expr)) {}

    constexpr auto getElements() const noexcept -> const std::vector<ForExprElement>&
    {
        return elements_;
    }
    constexpr auto getElements() noexcept -> std::vector<ForExprElement>&
    {
        return elements_;
    }

    constexpr auto getReturnExpression() const noexcept -> const Expression&
    {
        return ret_expr_;
    }
    constexpr auto getReturnExpression() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    std::vector<ForExprElement> elements_;
    Expression ret_expr_;
};

} // namespace ast
