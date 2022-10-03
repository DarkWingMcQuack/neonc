#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <vector>

namespace ast {

class TupleExpr : public AreaBase
{
public:
    constexpr TupleExpr(lexing::TextArea area, std::vector<Expression>&& exprs) noexcept
        : AreaBase(area),
          expressions_(std::move(exprs)) {}

    constexpr TupleExpr() noexcept = delete;
    constexpr TupleExpr(const TupleExpr&) noexcept = delete;
    constexpr TupleExpr(TupleExpr&&) noexcept = default;
    constexpr auto operator=(const TupleExpr&) noexcept -> TupleExpr& = delete;
    constexpr auto operator=(TupleExpr&&) noexcept -> TupleExpr& = default;

    constexpr auto operator==(const TupleExpr& other) const noexcept -> bool
    {
        return expressions_ == other.expressions_;
    }

    constexpr auto getExpressions() const noexcept -> const std::vector<Expression>&
    {
        return expressions_;
    }
    constexpr auto getExpressions() noexcept -> std::vector<Expression>&
    {
        return expressions_;
    }

private:
    std::vector<Expression> expressions_;
};

} // namespace ast
