#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <vector>

namespace ast {

class LambdaType : public AreaBase
{
public:
    constexpr LambdaType(lexing::TextArea area, std::vector<Type>&& arguments, Type&& return_type) noexcept
        : AreaBase(area),
          arguments_(std::move(arguments)),
          return_(std::move(return_type)) {}

    constexpr auto operator==(const LambdaType& other) const noexcept
        -> bool
    {
        return arguments_ == other.arguments_ and return_ == other.return_;
    }

    constexpr auto numberOfArguments() const noexcept -> std::size_t
    {
        return arguments_.size();
    }

    constexpr auto getArguments() const noexcept -> const std::vector<Type>&
    {
        return arguments_;
    }
    constexpr auto getArguments() noexcept -> std::vector<Type>&
    {
        return arguments_;
    }

    constexpr auto getReturnType() const noexcept -> const Type&
    {
        return return_;
    }
    constexpr auto getReturnType() noexcept -> Type&
    {
        return return_;
    }

private:
    std::vector<Type> arguments_;
    Type return_;
};

} // namespace ast
