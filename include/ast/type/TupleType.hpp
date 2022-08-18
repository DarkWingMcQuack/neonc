#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <vector>

namespace ast {

class TupleType : public AreaBase
{
public:
    TupleType(lexing::TextArea area, std::vector<Type>&& types) noexcept
        : AreaBase(area),
          types_(std::move(types)) {}

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
