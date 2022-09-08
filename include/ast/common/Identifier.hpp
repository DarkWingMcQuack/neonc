#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>
#include <utility>
#include <variant>

namespace ast {

class Identifier : public AreaBase
{
public:
    constexpr Identifier(lexing::TextArea area, std::string_view value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}

    constexpr auto getValue() const noexcept -> std::string_view
    {
        return value_;
    }

private:
    std::string_view value_;
};

} // namespace ast
