#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace ast {

class FunctionCall : public AreaBase
{
public:
    constexpr FunctionCall(lexing::TextArea area,
                           Expression&& caller,
                           std::vector<Expression>&& arguments) noexcept
        : AreaBase{std::move(area)},
          caller_(std::move(caller)),
          arguments_(std::move(arguments)) {}

    constexpr FunctionCall() noexcept = delete;
    constexpr FunctionCall(const FunctionCall&) noexcept = delete;
    constexpr FunctionCall(FunctionCall&&) noexcept = default;
    constexpr auto operator=(const FunctionCall&) noexcept -> FunctionCall& = delete;
    constexpr auto operator=(FunctionCall&&) noexcept -> FunctionCall& = default;

    constexpr auto operator==(const FunctionCall& other) const noexcept -> bool
    {
        return caller_ == other.caller_ and arguments_ == other.arguments_;
    }

    constexpr auto getCaller() const noexcept -> const Expression&
    {
        return caller_;
    }
    constexpr auto getCaller() noexcept -> Expression&
    {
        return caller_;
    }

    constexpr auto getArguments() const noexcept
        -> const std::vector<Expression>&
    {
        return arguments_;
    }
    constexpr auto getArguments() noexcept -> std::vector<Expression>&
    {
        return arguments_;
    }

private:
    Expression caller_;
    std::vector<Expression> arguments_;
};

} // namespace ast
