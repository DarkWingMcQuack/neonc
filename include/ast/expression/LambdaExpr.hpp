#pragma once

#include <ast/Ast.hpp>
#include <ast/common/Identifier.hpp>
#include <optional>

namespace ast {

class LambdaParameter : public AreaBase
{
public:
    LambdaParameter(lexing::TextArea area,
                    Identifier&& name,
                    Type&& type) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          type_(std::move(type)) {}

    LambdaParameter(lexing::TextArea area,
                    Identifier&& name) noexcept
        : AreaBase(area),
          name_(std::move(name)) {}

    auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    auto hasType() const noexcept -> bool
    {
        return type_.has_value();
    }

    auto getType() const noexcept -> const std::optional<Type>&
    {
        return type_;
    }
    auto getType() noexcept -> std::optional<Type>&
    {
        return type_;
    }

private:
    Identifier name_;
    std::optional<Type> type_;
};

class LambdaExpr : public AreaBase
{
public:
    LambdaExpr(lexing::TextArea area,
               std::vector<LambdaParameter>&& parameters,
               Type&& ret_type,
               std::vector<Statement>&& body,
               Expression&& ret_expr) noexcept
        : AreaBase(area),
          parameters_(std::move(parameters)),
          ret_type_(std::move(ret_type)),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    LambdaExpr(lexing::TextArea area,
               std::vector<LambdaParameter>&& parameters,
               std::vector<Statement>&& body,
               Expression&& ret_expr) noexcept
        : AreaBase(area),
          parameters_(std::move(parameters)),
          ret_type_(std::nullopt),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    auto getParameters() const noexcept -> const std::vector<LambdaParameter>&
    {
        return parameters_;
    }
    auto getParameters() noexcept -> std::vector<LambdaParameter>&
    {
        return parameters_;
    }

    auto hasReturnType() const noexcept -> bool
    {
        return ret_type_.has_value();
    }

    auto getReturnType() const noexcept -> const std::optional<Type>&
    {
        return ret_type_;
    }
    auto getReturnType() noexcept -> std::optional<Type>&
    {
        return ret_type_;
    }

    auto getBody() const noexcept -> const std::vector<Statement>&
    {
        return body_;
    }
    auto getBody() noexcept -> std::vector<Statement>&
    {
        return body_;
    }

    auto getReturnExpr() const noexcept -> const Expression&
    {
        return ret_expr_;
    }
    auto getReturnExpr() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    std::vector<LambdaParameter> parameters_;
    std::optional<Type> ret_type_;
    std::vector<Statement> body_;
    Expression ret_expr_;
};

} // namespace ast
