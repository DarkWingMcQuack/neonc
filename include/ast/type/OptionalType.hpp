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
    OptionalType(lexing::TextArea area, Type&& type) noexcept
        : AreaBase(area),
          type_(std::move(type)) {}


    auto getType() const noexcept -> const Type&
    {
        return type_;
    }
    auto getType() noexcept -> Type&
    {
        return type_;
    }

private:
    Type type_;
};

} // namespace ast
