#pragma once

#include <ast/common/AreaBase.hpp>
#include <utility>

namespace ast {

class Self : public AreaBase
{
public:
    constexpr Self(lexing::TextArea area) noexcept
        : AreaBase{std::move(area)} {}

    constexpr auto operator==(const Self&) const noexcept
        -> bool
    {
        return true;
    }
};

} // namespace ast
