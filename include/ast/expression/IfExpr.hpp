#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace ast {

class ElifExpr : public AreaBase
{
public:
    ElifExpr(lexing::TextArea area,
             Expression&& condition,
             std::vector<Statement>&& body,
             Expression&& ret_expr) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }
    auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

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
    Expression condition_;
    std::vector<Statement> body_;
    Expression ret_expr_;
};

class IfExpr : public AreaBase
{
public:
    IfExpr(lexing::TextArea area,
           Expression&& condition,
           std::vector<Statement>&& body,
           Expression&& ret_expr,
           std::vector<ElifExpr>&& elifs,
           std::vector<Statement>&& else_body,
           Expression&& else_ret_expr) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)),
          elifs_(std::move(elifs)),
          else_body_(std::move(else_body)),
          else_ret_expr_(std::move(else_ret_expr)) {}

    auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }
    auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

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

    auto getElifs() const noexcept -> const std::vector<ElifExpr>&
    {
        return elifs_;
    }
    auto getElifs() noexcept -> std::vector<ElifExpr>&
    {
        return elifs_;
    }

    auto getElseBody() const noexcept -> const std::vector<Statement>&
    {
        return else_body_;
    }
    auto getElseBody() noexcept -> std::vector<Statement>&
    {
        return else_body_;
    }

    auto getElseReturnExpression() const noexcept -> const Expression&
    {
        return else_ret_expr_;
    }
    auto getElseReturnExpression() noexcept -> Expression&
    {
        return else_ret_expr_;
    }

private:
    Expression condition_;
    std::vector<Statement> body_;
    Expression ret_expr_;
    std::vector<ElifExpr> elifs_;
    std::vector<Statement> else_body_;
    Expression else_ret_expr_;
};

} // namespace ast
