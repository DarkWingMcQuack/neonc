#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class ForLetElement : public AreaBase
{
public:
    constexpr ForLetElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    constexpr ForLetElement() noexcept = delete;
    constexpr ForLetElement(const ForLetElement&) noexcept = delete;
    constexpr ForLetElement(ForLetElement&&) noexcept = default;
    constexpr auto operator=(const ForLetElement&) noexcept -> ForLetElement& = delete;
    constexpr auto operator=(ForLetElement&&) noexcept -> ForLetElement& = default;

    constexpr auto operator==(const ForLetElement& other) const noexcept -> bool
    {
        return name_ == other.name_ and rhs_ == other.rhs_;
    }

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_;
    Expression rhs_;
};

class ForMonadicElement : public AreaBase
{
public:
    constexpr ForMonadicElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    constexpr ForMonadicElement() noexcept = delete;
    constexpr ForMonadicElement(const ForMonadicElement&) noexcept = delete;
    constexpr ForMonadicElement(ForMonadicElement&&) noexcept = default;
    constexpr auto operator=(const ForMonadicElement&) noexcept -> ForMonadicElement& = delete;
    constexpr auto operator=(ForMonadicElement&&) noexcept -> ForMonadicElement& = default;

    constexpr auto operator==(const ForMonadicElement& other) const noexcept -> bool
    {
        return name_ == other.name_ and rhs_ == other.rhs_;
    }

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_;
    Expression rhs_;
};

using ForElement = std::variant<ForLetElement, ForMonadicElement>;

} // namespace ast
