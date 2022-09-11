#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>

namespace ast {

class String : public AreaBase
{
public:
    constexpr String(lexing::TextArea area, std::string_view value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto operator==(const String& other) const noexcept
        -> bool
    {
        return value_ == other.value_;
    }

    constexpr auto getValue() const noexcept -> std::string_view
    {
        return value_;
    }

private:
    std::string_view value_;
};

} // namespace ast
