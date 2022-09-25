#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class UnaryOperation : public AreaBase
{
public:
	constexpr UnaryOperation(lexing::TextArea area, Expression&& rhs) noexcept
		: AreaBase(area),
		  rhs_(std::move(rhs)) {}

	constexpr UnaryOperation() noexcept = delete;
	constexpr UnaryOperation(UnaryOperation&&) noexcept = default;
	constexpr UnaryOperation(const UnaryOperation&) noexcept = delete;
	constexpr auto operator=(UnaryOperation&&) noexcept -> UnaryOperation& = default;
	constexpr auto operator=(const UnaryOperation&) noexcept -> UnaryOperation& = delete;

	constexpr virtual ~UnaryOperation() noexcept = 0;

	constexpr auto getRightHandSide() const noexcept -> const Expression&
	{
		return rhs_;
	}
	constexpr auto getRightHandSide() noexcept -> Expression&
	{
		return rhs_;
	}

private:
	Expression rhs_;
};

constexpr UnaryOperation::~UnaryOperation() noexcept = default;

class UnaryPlus : public UnaryOperation
{
	using UnaryOperation::UnaryOperation;
	using UnaryOperation::operator=;
};

class UnaryMinus : public UnaryOperation
{
	using UnaryOperation::UnaryOperation;
	using UnaryOperation::operator=;
};

class LogicalNot : public UnaryOperation
{
	using UnaryOperation::UnaryOperation;
	using UnaryOperation::operator=;
};

} // namespace ast
