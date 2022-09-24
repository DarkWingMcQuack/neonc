#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>

namespace ast {

class WhileStmt : public AreaBase
{
public:
    constexpr WhileStmt(lexing::TextArea area, Expression&& condition, std::vector<Statement>&& body) noexcept
        : AreaBase(area),
          condition_(std::move(condition)),
          body_(std::move(body)) {}

    constexpr auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }
    constexpr auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

    constexpr auto getBody() const noexcept -> const std::vector<Statement>&
    {
        return body_;
    }
    constexpr auto getBody() noexcept -> std::vector<Statement>&
    {
        return body_;
    }

private:
    Expression condition_;
    std::vector<Statement> body_;
};

} // namespace ast
