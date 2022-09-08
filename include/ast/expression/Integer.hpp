#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <utility>

namespace ast {

class Integer : AreaBase
{
public:
    constexpr Integer(lexing::TextArea area, std::int64_t value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto operator==(const Integer& other) const noexcept
        -> bool
    {
        return value_ == other.value_;
    }

    constexpr auto getValue() const noexcept -> std::int64_t
    {
        return value_;
    }

private:
    std::int64_t value_;
};

} // namespace ast
