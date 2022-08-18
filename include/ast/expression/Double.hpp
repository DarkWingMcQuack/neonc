#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>
#include <utility>

namespace ast {

class Double : AreaBase
{
public:
    constexpr Double(lexing::TextArea area, double value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto getValue() const noexcept -> double
    {
        return value_;
    }

private:
    double value_;
};

} // namespace ast
