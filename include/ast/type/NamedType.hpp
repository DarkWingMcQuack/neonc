#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <vector>

namespace ast {

class NamedType : public AreaBase
{
public:
    constexpr NamedType(lexing::TextArea area, std::vector<Identifier>&& namespce, Identifier&& type) noexcept
        : AreaBase(area),
          namespce_(std::move(namespce)),
          type_(std::move(type)) {}

    constexpr auto getNamespace() const noexcept -> const std::vector<Identifier>&
    {
        return namespce_;
    }
    constexpr auto getNamespace() noexcept -> std::vector<Identifier>&
    {
        return namespce_;
    }

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return type_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return type_;
    }

private:
    std::vector<Identifier> namespce_;
    Identifier type_;
};

} // namespace ast
