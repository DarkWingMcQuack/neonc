#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <vector>

namespace ast {

class UnionType : public AreaBase
{
public:
    UnionType(lexing::TextArea area, std::vector<Type>&& types) noexcept
        : AreaBase(area),
          types_(std::move(types)) {}

    constexpr auto operator==(const UnionType& other) const noexcept
        -> bool
    {
        return types_ == other.types_;
    }

    auto size() const noexcept -> std::size_t
    {
        return types_.size();
    }

    auto getTypes() const noexcept -> const std::vector<Type>&
    {
        return types_;
    }
    auto getTypes() noexcept -> std::vector<Type>&
    {
        return types_;
    }

private:
    std::vector<Type> types_;
};

} // namespace ast
