#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>

namespace ast {

class Boolean : AreaBase
{
public:
    constexpr Boolean(lexing::TextArea area, bool value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto getValue() const noexcept -> bool
    {
        return value_;
    }

private:
    bool value_;
};

} // namespace ast
