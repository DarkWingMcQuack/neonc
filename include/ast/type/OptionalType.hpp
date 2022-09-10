#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <vector>

namespace ast {

class OptionalType : public AreaBase
{
public:
    constexpr OptionalType(lexing::TextArea area, Type&& type) noexcept
        : AreaBase(area),
          type_(std::move(type)) {}

    constexpr OptionalType(OptionalType&&) noexcept = default;
    constexpr OptionalType(const OptionalType&) noexcept = delete;
    constexpr auto operator=(OptionalType&&) -> OptionalType& = default;
    constexpr auto operator=(const OptionalType&) -> OptionalType& = delete;

    constexpr auto operator==(const OptionalType& other) const noexcept
        -> bool
    {
        return type_ == other.type_;
    }


    constexpr auto getType() const noexcept -> const Type&
    {
        return type_;
    }
    constexpr auto getType() noexcept -> Type&
    {
        return type_;
    }

private:
    Type type_;
};

} // namespace ast
