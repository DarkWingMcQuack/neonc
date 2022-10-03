#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace ast {

class ElifStmt : public AreaBase
{
public:
	constexpr ElifStmt(lexing::TextArea area,
					   Expression&& condition,
					   std::vector<ast::Statement>&& body) noexcept
		: AreaBase(std::move(area)),
		  condition_(std::move(condition)),
		  body_(std::move(body)) {}

	constexpr ElifStmt() noexcept = delete;
	constexpr ElifStmt(const ElifStmt&) noexcept = delete;
	constexpr ElifStmt(ElifStmt&&) noexcept = default;
	constexpr auto operator=(const ElifStmt&) noexcept -> ElifStmt& = delete;
	constexpr auto operator=(ElifStmt&&) noexcept -> ElifStmt& = default;

	constexpr auto operator==(const ElifStmt& other) const noexcept -> bool
	{
		return condition_ == other.condition_ and body_ == other.body_;
	}

	constexpr auto getCondition() const noexcept -> const Expression&
	{
		return condition_;
	}
	constexpr auto getCondition() noexcept -> Expression&
	{
		return condition_;
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
	Expression condition_;
	std::vector<ast::Statement> body_;
};

class ElseStmt : public AreaBase
{
public:
	constexpr ElseStmt(lexing::TextArea area, std::vector<ast::Statement>&& body) noexcept
		: AreaBase(std::move(area)),
		  body_(std::move(body)) {}

	constexpr ElseStmt() noexcept = delete;
	constexpr ElseStmt(const ElseStmt&) noexcept = delete;
	constexpr ElseStmt(ElseStmt&&) noexcept = default;
	constexpr auto operator=(const ElseStmt&) noexcept -> ElseStmt& = delete;
	constexpr auto operator=(ElseStmt&&) noexcept -> ElseStmt& = default;

	constexpr auto operator==(const ElseStmt& other) const noexcept -> bool
	{
		return body_ == other.body_;
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
	std::vector<ast::Statement> body_;
};

class IfStmt : public AreaBase
{
public:
	constexpr IfStmt(lexing::TextArea area,
					 Expression&& condition,
					 std::vector<ast::Statement>&& body,
					 std::vector<ElifStmt>&& elifs,
					 std::optional<ElseStmt>&& else_) noexcept
		: AreaBase(std::move(area)),
		  condition_(std::move(condition)),
		  body_(std::move(body)),
		  elifs_(std::move(elifs)),
		  else_(std::move(else_)) {}

	constexpr IfStmt() noexcept = delete;
	constexpr IfStmt(const IfStmt&) noexcept = delete;
	constexpr IfStmt(IfStmt&&) noexcept = default;
	constexpr auto operator=(const IfStmt&) noexcept -> IfStmt& = delete;
	constexpr auto operator=(IfStmt&&) noexcept -> IfStmt& = default;

	constexpr auto operator==(const IfStmt& other) const noexcept -> bool
	{
		return condition_ == other.condition_
			and body_ == other.body_
			and elifs_ == other.elifs_
			and else_ == other.else_;
	}

	constexpr auto getCondition() const noexcept -> const Expression&
	{
		return condition_;
	}

	constexpr auto getCondition() noexcept -> Expression&
	{
		return condition_;
	}

	constexpr auto getBody() const noexcept -> const std::vector<ast::Statement>&
	{
		return body_;
	}
	constexpr auto getBody() noexcept -> std::vector<ast::Statement>&
	{
		return body_;
	}

	constexpr auto getElifs() const noexcept -> const std::vector<ElifStmt>&
	{
		return elifs_;
	}
	constexpr auto getElifs() noexcept -> std::vector<ElifStmt>&
	{
		return elifs_;
	}

	constexpr auto getElse() const noexcept -> const std::optional<ElseStmt>&
	{
		return else_;
	}
	constexpr auto getElse() noexcept -> std::optional<ElseStmt>&
	{
		return else_;
	}

private:
	Expression condition_;
	std::vector<ast::Statement> body_;
	std::vector<ElifStmt> elifs_;
	std::optional<ElseStmt> else_;
};

} // namespace ast
