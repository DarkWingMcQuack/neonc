#pragma once

#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <vector>

namespace ast {

class DirectImport : public AreaBase
{
public:
    DirectImport(lexing::TextArea area,
                 std::vector<Identifier>&& namespce,
                 Identifier element) noexcept
        : AreaBase(area),
          namespce_(std::move(namespce)),
          imported_element_(element) {}

    constexpr auto operator==(const DirectImport& other) const noexcept
        -> bool
    {
        return namespce_ == other.namespce_ and imported_element_ == other.imported_element_;
    }

    auto getNamespace() const noexcept -> const std::vector<Identifier>&
    {
        return namespce_;
    }
    auto getNamespace() noexcept -> std::vector<Identifier>&
    {
        return namespce_;
    }

    auto getImportedElement() const noexcept -> const Identifier&
    {
        return imported_element_;
    }

    auto getImportedElement() noexcept -> Identifier&
    {
        return imported_element_;
    }

private:
    std::vector<Identifier> namespce_;
    Identifier imported_element_;
};

} // namespace ast
