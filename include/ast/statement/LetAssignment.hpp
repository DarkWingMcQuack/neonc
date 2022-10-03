#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class LetAssignment : public AreaBase
{
public:
    constexpr LetAssignment(lexing::TextArea area,
                            Identifier&& name,
                            std::optional<Type>&& type,
                            Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          type_(std::move(type)),
          rhs_(std::move(rhs)) {}

    constexpr LetAssignment() noexcept = delete;
    constexpr LetAssignment(const LetAssignment&) noexcept = delete;
    constexpr LetAssignment(LetAssignment&&) noexcept = default;
    constexpr auto operator=(const LetAssignment&) noexcept -> LetAssignment& = delete;
    constexpr auto operator=(LetAssignment&&) noexcept -> LetAssignment& = default;

    constexpr auto operator==(const LetAssignment& other) const noexcept -> bool
    {
        return name_ == other.name_
            and type_ == other.type_
            and rhs_ == other.rhs_;
    }

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto hasType() const noexcept -> bool
    {
        return type_.has_value();
    }

    constexpr auto getType() const noexcept -> const std::optional<Type>&
    {
        return type_;
    }
    constexpr auto getType() noexcept -> std::optional<Type>&
    {
        return type_;
    }
    constexpr auto setType(ast::Type&& type) noexcept
    {
        type_ = std::move(type);
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
    std::optional<Type> type_;
    Expression rhs_;
};

} // namespace ast
