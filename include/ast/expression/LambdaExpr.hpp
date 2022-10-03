#pragma once

#include <ast/Ast.hpp>
#include <ast/common/Identifier.hpp>
#include <optional>

namespace ast {

class LambdaParameter : public AreaBase
{
public:
    constexpr LambdaParameter(Identifier&& name,
                              Type&& type) noexcept
        : AreaBase(lexing::TextArea::combine(name.getArea(), getTextArea(type))),
          name_(std::move(name)),
          type_(std::move(type)) {}

    constexpr LambdaParameter(Identifier&& name) noexcept
        : AreaBase(name.getArea()),
          name_(std::move(name)) {}

    constexpr LambdaParameter() noexcept = delete;
    constexpr LambdaParameter(const LambdaParameter&) noexcept = delete;
    constexpr LambdaParameter(LambdaParameter&&) noexcept = default;
    constexpr auto operator=(const LambdaParameter&) noexcept -> LambdaParameter& = delete;
    constexpr auto operator=(LambdaParameter&&) noexcept -> LambdaParameter& = default;

    constexpr auto operator==(const LambdaParameter& other) const noexcept -> bool
    {
        return name_ == other.name_ and type_ == other.type_;
    }

    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    constexpr auto hasType() const noexcept -> bool
    {
        return type_.has_value();
    }

    constexpr auto getType() const noexcept -> const std::optional<Type>&
    {
        return type_;
    }
    constexpr auto getType() noexcept -> std::optional<Type>&
    {
        return type_;
    }
    constexpr auto setType(ast::Type&& type) noexcept
    {
        type_ = std::move(type);
    }

private:
    Identifier name_;
    std::optional<Type> type_;
};

class LambdaExpr : public AreaBase
{
public:
    constexpr LambdaExpr(lexing::TextArea area,
                         std::vector<LambdaParameter>&& parameters,
                         Type&& ret_type,
                         Expression&& ret_expr) noexcept
        : AreaBase(area),
          parameters_(std::move(parameters)),
          ret_type_(std::move(ret_type)),
          ret_expr_(std::move(ret_expr)) {}

    constexpr LambdaExpr(lexing::TextArea area,
                         std::vector<LambdaParameter>&& parameters,
                         Expression&& ret_expr) noexcept
        : AreaBase(area),
          parameters_(std::move(parameters)),
          ret_type_(std::nullopt),
          ret_expr_(std::move(ret_expr)) {}

    constexpr LambdaExpr() noexcept = delete;
    constexpr LambdaExpr(const LambdaExpr&) noexcept = delete;
    constexpr LambdaExpr(LambdaExpr&&) noexcept = default;
    constexpr auto operator=(const LambdaExpr&) noexcept -> LambdaExpr& = delete;
    constexpr auto operator=(LambdaExpr&&) noexcept -> LambdaExpr& = default;

    constexpr auto operator==(const LambdaExpr& other) const noexcept -> bool
    {
        return parameters_ == other.parameters_
            and ret_type_ == other.ret_type_
            and ret_expr_ == other.ret_expr_;
    }

    constexpr auto getParameters() const noexcept -> const std::vector<LambdaParameter>&
    {
        return parameters_;
    }
    constexpr auto getParameters() noexcept -> std::vector<LambdaParameter>&
    {
        return parameters_;
    }

    constexpr auto hasReturnType() const noexcept -> bool
    {
        return ret_type_.has_value();
    }

    constexpr auto getReturnType() const noexcept -> const std::optional<Type>&
    {
        return ret_type_;
    }
    constexpr auto getReturnType() noexcept -> std::optional<Type>&
    {
        return ret_type_;
    }

    constexpr auto getReturnExpr() const noexcept -> const Expression&
    {
        return ret_expr_;
    }
    constexpr auto getReturnExpr() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    std::vector<LambdaParameter> parameters_;
    std::optional<Type> ret_type_;
    Expression ret_expr_;
};

} // namespace ast
