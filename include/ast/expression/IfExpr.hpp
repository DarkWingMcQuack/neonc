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
    constexpr ElifExpr(lexing::TextArea area,
                       Expression&& condition,
                       Expression&& block) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          block_(std::move(block)) {}

	constexpr ElifExpr() noexcept = delete;
	constexpr ElifExpr(const ElifExpr&) noexcept = delete;
	constexpr ElifExpr(ElifExpr&&) noexcept = default;
	constexpr auto operator=(const ElifExpr&) noexcept -> ElifExpr& = delete;
	constexpr auto operator=(ElifExpr&&) noexcept -> ElifExpr& = default;


    constexpr auto operator==(const ElifExpr& other) const noexcept -> bool
    {
        return condition_ == other.condition_ and block_ == other.block_;
    }

    constexpr auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }
    constexpr auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

    constexpr auto getBody() const noexcept -> const Expression&
    {
        return block_;
    }
    constexpr auto getBody() noexcept -> Expression&
    {
        return block_;
    }

private:
    Expression condition_;
    Expression block_;
};

class IfExpr : public AreaBase
{
public:
    constexpr IfExpr(lexing::TextArea area,
                     Expression&& condition,
                     Expression&& body,
                     std::vector<ElifExpr>&& elifs,
                     Expression&& else_body) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          body_(std::move(body)),
          elifs_(std::move(elifs)),
          else_body_(std::move(else_body)) {}

	constexpr IfExpr() noexcept = delete;
	constexpr IfExpr(const IfExpr&) noexcept = delete;
	constexpr IfExpr(IfExpr&&) noexcept = default;
	constexpr auto operator=(const IfExpr&) noexcept -> IfExpr& = delete;
	constexpr auto operator=(IfExpr&&) noexcept -> IfExpr& = default;

    constexpr auto operator==(const IfExpr& other) const noexcept -> bool
    {
        return condition_ == other.condition_
            and body_ == other.body_
            and elifs_ == other.elifs_
            and else_body_ == other.else_body_;
    }

    constexpr auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }

    constexpr auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

    constexpr auto getBody() const noexcept -> const Expression&
    {
        return body_;
    }

    constexpr auto getBody() noexcept -> Expression&
    {
        return body_;
    }

    constexpr auto getElifs() const noexcept -> const std::vector<ElifExpr>&
    {
        return elifs_;
    }
    constexpr auto getElifs() noexcept -> std::vector<ElifExpr>&
    {
        return elifs_;
    }

    constexpr auto getElseBody() const noexcept -> const Expression&
    {
        return else_body_;
    }
    constexpr auto getElseBody() noexcept -> Expression&
    {
        return else_body_;
    }

private:
    Expression condition_;
    Expression body_;
    std::vector<ElifExpr> elifs_;
    Expression else_body_;
};

} // namespace ast
