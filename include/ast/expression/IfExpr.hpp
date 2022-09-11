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
                       BlockExpr&& block) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          block_(std::move(block)) {}

    constexpr auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }
    constexpr auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

    constexpr auto getBlock() const noexcept -> const BlockExpr&
    {
        return block_;
    }
    constexpr auto getBlock() noexcept -> BlockExpr&
    {
        return block_;
    }

private:
    Expression condition_;
    BlockExpr block_;
};

class IfExpr : public AreaBase
{
public:
    constexpr IfExpr(lexing::TextArea area,
                     Expression&& condition,
                     BlockExpr&& if_block,
                     std::vector<ElifExpr>&& elifs,
                     BlockExpr&& else_block) noexcept
        : AreaBase(std::move(area)),
          condition_(std::move(condition)),
          if_block_(std::move(if_block)),
          elifs_(std::move(elifs)),
          else_block_(std::move(else_block)) {}

    constexpr auto getCondition() const noexcept -> const Expression&
    {
        return condition_;
    }

    constexpr auto getCondition() noexcept -> Expression&
    {
        return condition_;
    }

    constexpr auto getIfBlock() const noexcept -> const BlockExpr&
    {
        return if_block_;
    }

    constexpr auto getIfBlock() noexcept -> BlockExpr&
    {
        return if_block_;
    }

    constexpr auto getElifs() const noexcept -> const std::vector<ElifExpr>&
    {
        return elifs_;
    }
    constexpr auto getElifs() noexcept -> std::vector<ElifExpr>&
    {
        return elifs_;
    }

    constexpr auto getElseBlock() const noexcept -> const BlockExpr&
    {
        return else_block_;
    }
    constexpr auto getElseBlock() noexcept -> BlockExpr&
    {
        return else_block_;
    }

private:
    Expression condition_;
    BlockExpr if_block_;
    std::vector<ElifExpr> elifs_;
    BlockExpr else_block_;
};

} // namespace ast
