
#pragma once

#include <ast/Ast.hpp>
#include <memory>
#include <vector>

namespace ast {

class BlockExpr : public AreaBase
{
public:
    constexpr BlockExpr(lexing::TextArea area,
						std::vector<Statement>&& body,
						Expression&& ret_expr) noexcept
        : AreaBase(area),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    constexpr BlockExpr() noexcept = delete;
    constexpr BlockExpr(const BlockExpr&) noexcept = delete;
    constexpr BlockExpr(BlockExpr&&) noexcept = default;
    constexpr auto operator=(BlockExpr&&) noexcept -> BlockExpr& = default;
    constexpr auto operator=(const BlockExpr&) noexcept -> BlockExpr& = delete;

    constexpr auto operator==(const BlockExpr& other) const noexcept -> bool
    {
        return body_ == other.body_ and ret_expr_ == other.ret_expr_;
    }

    constexpr auto getBody() const noexcept -> const std::vector<Statement>&
    {
        return body_;
    }
    constexpr auto getBody() noexcept -> std::vector<Statement>&
    {
        return body_;
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
    std::vector<Statement> body_;
    Expression ret_expr_;
};

} // namespace ast
