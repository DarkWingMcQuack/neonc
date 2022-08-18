#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <vector>

namespace ast {

class TypeclassImport : public AreaBase
{
public:
    TypeclassImport(lexing::TextArea area,
                    std::vector<Identifier>&& namespce,
                    Identifier&& typeclass,
                    Type&& instance) noexcept
        : AreaBase(area),
          namespce_(std::move(namespce)),
          typeclass_(std::move(typeclass)),
          instance_(std::move(instance)) {}

    auto getNamespace() const noexcept -> const std::vector<Identifier>&
    {
        return namespce_;
    }
    auto getNamespace() noexcept -> std::vector<Identifier>&
    {
        return namespce_;
    }

    auto getTypeclass() const noexcept -> const Identifier&
    {
        return typeclass_;
    }
    auto getTypeclass() noexcept -> Identifier&
    {
        return typeclass_;
    }

    auto getInstanceType() const noexcept -> const Type&
    {
        return instance_;
    }
    auto getInstanceType() noexcept -> Type&
    {
        return instance_;
    }

private:
    std::vector<Identifier> namespce_;
    Identifier typeclass_;
    Type instance_;
};


} // namespace ast
