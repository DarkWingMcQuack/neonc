#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class ReturnStmt : public AreaBase
{
public:
    ReturnStmt(lexing::TextArea area, Expression&& ret_expr) noexcept
        : AreaBase(area),
          ret_expr_(std::move(ret_expr)) {}

    auto getReturnExpression() const noexcept -> const Expression&
    {
        return ret_expr_;
    }
    auto getReturnExpression() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    Expression ret_expr_;
};

} // namespace ast
