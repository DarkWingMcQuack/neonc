#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class BinaryOperation : public AreaBase
{
public:
    constexpr BinaryOperation(lexing::TextArea area, Expression&& lhs, Expression&& rhs) noexcept
        : AreaBase(area),
          lhs_(std::move(lhs)),
          rhs_(std::move(rhs)) {}

    constexpr BinaryOperation() noexcept = delete;
    constexpr BinaryOperation(BinaryOperation&&) noexcept = default;
    constexpr BinaryOperation(const BinaryOperation&) noexcept = delete;
    constexpr auto operator=(BinaryOperation&&) noexcept -> BinaryOperation& = default;
    constexpr auto operator=(const BinaryOperation&) noexcept -> BinaryOperation& = delete;
    // pure virtual destructor which makes it impossible to instantiate an object
    constexpr virtual ~BinaryOperation() noexcept = 0;

    constexpr auto getLeftHandSide() const noexcept -> const Expression&
    {
        return lhs_;
    }
    constexpr auto getLeftHandSide() noexcept -> Expression&
    {
        return lhs_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

    template<typename T>
        requires std::is_base_of_v<BinaryOperation, T>
    friend constexpr auto operator==(const T& lhs, const T& rhs) noexcept
    {
        return lhs.lhs_ == rhs.lhs_ and lhs.rhs_ == rhs.rhs_;
    }

private:
    Expression lhs_;
    Expression rhs_;
};

constexpr BinaryOperation::~BinaryOperation() noexcept = default;




class Addition : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class BitwiseAnd : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class BitwiseOr : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class Division : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class Equal : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class GreaterEqThen : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class GreaterThen : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class LessEqThen : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class LessThen : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class LogicalAnd : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class LogicalOr : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class MemberAccess : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class Multiplication : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class NotEqual : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class Remainder : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

class Substraction : public ast::BinaryOperation
{
public:
    using BinaryOperation::BinaryOperation;
    using BinaryOperation::operator=;
};

} // namespace ast
