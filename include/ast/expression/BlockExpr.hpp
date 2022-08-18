
#pragma once

#include <ast/Ast.hpp>
#include <memory>
#include <vector>

namespace ast {

class BlockExpr : public AreaBase
{
public:
    BlockExpr(lexing::TextArea area, std::vector<Statement>&& body, Expression&& ret_expr) noexcept
        : AreaBase(area),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    auto getBody() const noexcept -> const std::vector<Statement>&
    {
        return body_;
    }
    auto getBody() noexcept -> std::vector<Statement>&
    {
        return body_;
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
    std::vector<Statement> body_;
    Expression ret_expr_;
};

} // namespace ast
