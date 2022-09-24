#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class MemberAccess : public AreaBase
{
public:
	constexpr MemberAccess(lexing::TextArea area, Expression&& lhs, Identifier&& rhs) noexcept
		: AreaBase(area),
		  lhs_(std::move(lhs)),
		  rhs_(std::move(rhs)) {}

	constexpr auto getLeftHandSide() const noexcept -> const Expression&
	{
		return lhs_;
	}
	constexpr auto getLeftHandSide() noexcept -> Expression&
	{
		return lhs_;
	}

	constexpr auto getRightHandSide() const noexcept -> const Identifier&
	{
		return rhs_;
	}
	constexpr auto getRightHandSide() noexcept -> Identifier&
	{
		return rhs_;
	}

private:
	Expression lhs_;
	Identifier rhs_;
};

} // namespace ast
