#pragma once

#include <ast/Ast.hpp>
#include <vector>
#include <ast/common/AreaBase.hpp>

namespace ast {

class TupleExpr : public AreaBase
{
public:
    TupleExpr(lexing::TextArea area, std::vector<Expression>&& exprs) noexcept
        : AreaBase(area),
          expressions_(std::move(exprs)) {}

    auto getExpressions() const noexcept -> const std::vector<Expression>&
    {
        return expressions_;
    }
    auto getExpressions() noexcept -> std::vector<Expression>&
    {
        return expressions_;
    }

private:
    std::vector<Expression> expressions_;
};

} // namespace ast
