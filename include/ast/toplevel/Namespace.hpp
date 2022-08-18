#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>

namespace ast {

class Namespace : AreaBase
{
public:
    Namespace(lexing::TextArea area,
              Identifier&& name,
              std::vector<ToplevelElement>&& elements) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          elements_(std::move(elements)) {}

    auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    auto getElements() const noexcept -> const std::vector<ToplevelElement>&
    {
        return elements_;
    }
    auto getElements() noexcept -> std::vector<ToplevelElement>&
    {
        return elements_;
    }


private:
    Identifier name_;
    std::vector<ToplevelElement> elements_;
};

} // namespace ast
