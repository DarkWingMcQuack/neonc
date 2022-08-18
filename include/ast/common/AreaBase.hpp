#pragma once

#include <lexer/TextArea.hpp>

namespace ast {

class AreaBase
{
public:
    constexpr AreaBase(lexing::TextArea area) noexcept
        : area_(area) {}
    constexpr virtual ~AreaBase() noexcept = default;

    [[nodiscard]] constexpr auto getArea() const noexcept -> lexing::TextArea
    {
        return area_;
    }

private:
    lexing::TextArea area_;
};

} // namespace ast
