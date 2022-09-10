#pragma once

#include <memory>
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

    constexpr auto setArea(lexing::TextArea area) noexcept -> void
    {
        area_ = std::move(area);
    }

private:
    lexing::TextArea area_;
};

} // namespace ast
