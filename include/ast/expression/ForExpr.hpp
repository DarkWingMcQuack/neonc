#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/ForElements.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {


class ForExpr : public AreaBase
{
public:
    constexpr ForExpr(lexing::TextArea area, std::vector<ForElement>&& elements, Expression&& ret_expr) noexcept
        : AreaBase(area),
          elements_(std::move(elements)),
          ret_expr_(std::move(ret_expr)) {}

    constexpr ForExpr() noexcept = delete;
    constexpr ForExpr(const ForExpr&) noexcept = delete;
    constexpr ForExpr(ForExpr&&) noexcept = default;
    constexpr auto operator=(const ForExpr&) noexcept -> ForExpr& = delete;
    constexpr auto operator=(ForExpr&&) noexcept -> ForExpr& = default;

    constexpr auto operator==(const ForExpr& other) const noexcept -> bool
    {
        return elements_ == other.elements_ and ret_expr_ == other.ret_expr_;
    }

    constexpr auto getElements() const noexcept -> const std::vector<ForElement>&
    {
        return elements_;
    }
    constexpr auto getElements() noexcept -> std::vector<ForElement>&
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
    std::vector<ForElement> elements_;
    Expression ret_expr_;
};

} // namespace ast
