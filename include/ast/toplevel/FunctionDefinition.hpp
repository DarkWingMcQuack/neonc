#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>

namespace ast {

class FunctionParameter : AreaBase
{
public:
    FunctionParameter(lexing::TextArea area, Identifier&& name, Type&& type) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          type_(std::move(type)) {}

    auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    auto getType() const noexcept -> const Type&
    {
        return type_;
    }
    auto getType() noexcept -> Type&
    {
        return type_;
    }

private:
    Identifier name_;
    Type type_;
};

class FunctionDefinition : AreaBase
{
public:
    FunctionDefinition(lexing::TextArea area,
                       Identifier&& name,
                       std::vector<FunctionParameter>&& parameters,
                       Type&& return_type,
                       std::vector<FunctionStatement>&& body) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          parameters_(std::move(parameters)),
          return_type(std::move(return_type)),
          body_(std::move(body)) {}

    auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }
    auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    auto getParameters() const noexcept -> const std::vector<FunctionParameter>&
    {
        return parameters_;
    }
    auto getParameters() noexcept -> std::vector<FunctionParameter>&
    {
        return parameters_;
    }

    auto getReturnType() const noexcept -> const Type&
    {
        return return_type;
    }
    auto getReturnType() noexcept -> Type&
    {
        return return_type;
    }

    auto getBody() const noexcept -> const std::vector<FunctionStatement>&
    {
        return body_;
    }
    auto getBody() noexcept -> std::vector<FunctionStatement>&
    {
        return body_;
    }

private:
    Identifier name_;
    std::vector<FunctionParameter> parameters_;
    Type return_type;
    std::vector<FunctionStatement> body_;
};

} // namespace ast
