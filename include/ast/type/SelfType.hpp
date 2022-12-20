#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <vector>

namespace ast {

class SelfType : public AreaBase
{
public:
    constexpr SelfType(lexing::TextArea area) noexcept
        : AreaBase(area) {}

    constexpr SelfType(SelfType&&) noexcept = default;
    constexpr SelfType(const SelfType&) noexcept = default;
    constexpr auto operator=(SelfType&&) -> SelfType& = default;
    constexpr auto operator=(const SelfType&) -> SelfType& = default;

    constexpr auto operator==(const SelfType& other) const noexcept
        -> bool
    {
        return true;
    }
};

} // namespace ast
