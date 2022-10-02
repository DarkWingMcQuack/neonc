#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class ForLetElement : public AreaBase
{
public:
	constexpr ForLetElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
		: AreaBase(area),
		  name_(std::move(name)),
		  rhs_(std::move(rhs)) {}

	constexpr auto getName() const noexcept -> const Identifier&
	{
		return name_;
	}
	constexpr auto getName() noexcept -> Identifier&
	{
		return name_;
	}

	constexpr auto getRightHandSide() const noexcept -> const Expression&
	{
		return rhs_;
	}
	constexpr auto getRightHandSide() noexcept -> Expression&
	{
		return rhs_;
	}

private:
	Identifier name_;
	Expression rhs_;
};

class ForMonadicElement : public AreaBase
{
public:
	constexpr ForMonadicElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
		: AreaBase(area),
		  name_(std::move(name)),
		  rhs_(std::move(rhs)) {}

	constexpr auto getName() const noexcept -> const Identifier&
	{
		return name_;
	}
	constexpr auto getName() noexcept -> Identifier&
	{
		return name_;
	}

	constexpr auto getRightHandSide() const noexcept -> const Expression&
	{
		return rhs_;
	}
	constexpr auto getRightHandSide() noexcept -> Expression&
	{
		return rhs_;
	}

private:
	Identifier name_;
	Expression rhs_;
};

using ForElement = std::variant<ForLetElement, ForMonadicElement>;

} // namespace ast
