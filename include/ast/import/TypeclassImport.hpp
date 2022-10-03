#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <vector>

namespace ast {

class TypeclassImport : public AreaBase
{
public:
    constexpr TypeclassImport(lexing::TextArea area,
                              std::vector<Identifier>&& namespce,
                              Identifier&& typeclass,
                              Type&& instance) noexcept
        : AreaBase(area),
          namespce_(std::move(namespce)),
          typeclass_(std::move(typeclass)),
          instance_(std::move(instance)) {}

    constexpr TypeclassImport() noexcept = delete;
    constexpr TypeclassImport(const TypeclassImport&) noexcept = delete;
    constexpr TypeclassImport(TypeclassImport&&) noexcept = default;
    constexpr auto operator=(const TypeclassImport&) noexcept -> TypeclassImport& = delete;
    constexpr auto operator=(TypeclassImport&&) noexcept -> TypeclassImport& = default;
    constexpr auto operator==(const TypeclassImport& other) const noexcept -> bool
    {
        return namespce_ == other.namespce_
		  and typeclass_ == other.typeclass_
		  and instance_ == other.instance_;
    }

    constexpr auto getNamespace() const noexcept -> const std::vector<Identifier>&
    {
        return namespce_;
    }
    constexpr auto getNamespace() noexcept -> std::vector<Identifier>&
    {
        return namespce_;
    }

    constexpr auto getTypeclass() const noexcept -> const Identifier&
    {
        return typeclass_;
    }
    constexpr auto getTypeclass() noexcept -> Identifier&
    {
        return typeclass_;
    }

    constexpr auto getInstanceType() const noexcept -> const Type&
    {
        return instance_;
    }
    constexpr auto getInstanceType() noexcept -> Type&
    {
        return instance_;
    }

private:
    std::vector<Identifier> namespce_;
    Identifier typeclass_;
    Type instance_;
};


} // namespace ast
