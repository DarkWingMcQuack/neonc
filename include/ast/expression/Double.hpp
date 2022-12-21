#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>
#include <utility>

namespace ast {

class Double : public AreaBase
{
public:
    constexpr Double(lexing::TextArea area, double value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto operator==(const Double& other) const noexcept
        -> bool
    {
        // clang-format off
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wfloat-equal"
        return value_ == other.value_;
        #pragma GCC diagnostic pop
        // clang-format on
    }

    constexpr auto getValue() const noexcept -> double
    {
        return value_;
    }

private:
    double value_;
};

} // namespace ast
