#pragma once

#include <ast/common/AreaBase.hpp>
#include <utility>

namespace ast {

class Self : AreaBase
{
public:
    constexpr Self(lexing::TextArea area) noexcept
        : AreaBase{std::move(area)} {}
};

} // namespace ast
