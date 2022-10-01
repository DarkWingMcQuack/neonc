#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/ForElements.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {


class ForStmt : public AreaBase
{
public:
	constexpr ForStmt(lexing::TextArea area,
					  std::vector<ForElement>&& elements,
					  std::vector<ast::Statement>&& body) noexcept
		: AreaBase(area),
		  elements_(std::move(elements)),
		  body_(std::move(body)) {}

	constexpr auto getElements() const noexcept -> const std::vector<ForElement>&
	{
		return elements_;
	}
	constexpr auto getElements() noexcept -> std::vector<ForElement>&
	{
		return elements_;
	}

	constexpr auto getBody() const noexcept -> const std::vector<ast::Statement>&
	{
		return body_;
	}
	constexpr auto getBody() noexcept -> std::vector<ast::Statement>&
	{
		return body_;
	}


private:
	std::vector<ForElement> elements_;
	std::vector<ast::Statement> body_;
};

} // namespace ast
