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
    FunctionCall(lexing::TextArea area,
                 Expression&& caller,
                 std::vector<Expression>&& arguments) noexcept
        : AreaBase{std::move(area)},
          caller_(std::move(caller)),
          arguments_(std::move(arguments)) {}

    auto getCaller() const noexcept -> const Expression&
    {
        return caller_;
    }
    auto getCaller() noexcept -> Expression&
    {
        return caller_;
    }

    auto getArguments() const noexcept
        -> const std::vector<Expression>&
    {
        return arguments_;
    }
    auto getArguments() noexcept -> std::vector<Expression>&
    {
        return arguments_;
    }

private:
    Expression caller_;
    std::vector<Expression> arguments_;
};

} // namespace ast
