#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for statements,
// 1. while loop
// 2. let statment
// 3. any expression
// 8. if-else statement
template<class T>
class StatmentParser
{
public:
	constexpr auto statement() noexcept -> std::optional<ast::Statement>
	{
		if(lexer().next_is(lexing::TokenTypes::LET)) {
			return let();
		}

		if(lexer().next_is(lexing::TokenTypes::WHILE)) {
			return wile();
		}

		if(lexer().next_is(lexing::TokenTypes::IF)) {
			return ifStmt();
		}

		if(lexer().next_is(lexing::TokenTypes::FOR)) {
			return forStmt();
		}

		return expression();
	}

private:
	// parses tuples, literals, lambda expressions and (expressions)
	constexpr auto let() noexcept -> std::optional<ast::Statement>
	{
		// sanity check
		if(not lexer().next_is(lexing::TokenTypes::LET)) {
			return std::nullopt;
		}

		// pop the let token and get the area of it as start
		auto start = lexer().peek_and_pop().value().getArea();

		auto id_opt = identifier();
		if(not id_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto id = std::move(id_opt.value());

		// parse optional type annotation
		std::optional<ast::Type> t;
		if(lexer().next_is(lexing::TokenTypes::COLON)) {
			lexer().pop();

			auto type_opt = type();
			if(not type_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			t = std::move(type_opt.value());
		}

		if(not lexer().next_is(lexing::TokenTypes::ASSIGN)) {
			// TODO: return error "expected ="
			return std::nullopt;
		}

		auto rhs_opt = expression();
		if(not rhs_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto rhs = std::move(rhs_opt.value());

		auto end = ast::getTextArea(rhs);
		auto area = lexing::TextArea::combine(start, end);

		return ast::Statement{
			ast::forward<ast::LetAssignment>(area,
											 std::move(id),
											 std::move(t),
											 std::move(rhs))};
	}

	constexpr auto stmt_list() noexcept -> std::optional<std::vector<ast::Statement>>
	{
		std::vector<ast::Statement> stmts;

		while(true) {
			auto stmt_opt = statement();
			if(not stmt_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			stmts.emplace_back(stmt_opt.value());

			auto next_token_opt = lexer().peek_and_pop();
			if(not next_token_opt.has_value()) {
				// TODO: return expected a token from the lexer, got none this seems to be a lexer bug
				return std::nullopt;
			}
			auto next_token = std::move(next_token_opt.value());

			if(not next_token.isSeparator()) {
				// TODO: return error next token expected to be a ; or \n
				return std::nullopt;
			}
		}

		return stmts;
	}

	constexpr auto block() noexcept -> std::optional<std::pair<std::vector<ast::Statement>, lexing::TextArea>>
	{
		if(not lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
			// TODO: return expected {
			return std::nullopt;
		}
		auto start = lexer().peek_and_pop().value().getArea();

		auto body_opt = stmt_list();
		if(not body_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto body = std::move(body_opt.value());


		if(not lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
			// TODO: return expected }
			return std::nullopt;
		}

		auto end = lexer().peek_and_pop().value().getArea();
		auto area = lexing::TextArea::combine(start, end);

		return std::pair{std::move(body), std::move(area)};
	}

	constexpr auto wile() noexcept -> std::optional<ast::Statement>
	{
		// sanity check
		if(not lexer().next_is(lexing::TokenTypes::WHILE)) {
			// TODO: return expexted "while" token
			return std::nullopt;
		}

		auto start = lexer().peek_and_pop().value().getArea();

		auto condition_opt = expression();
		if(not condition_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto condition = std::move(condition_opt.value());

		auto body_opt = block();
		if(not body_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto [body, end] = std::move(body_opt.value());
		auto area = lexing::TextArea::combine(start, end);

		return ast::Statement{
			ast::forward<ast::WhileStmt>(area,
										 std::move(condition),
										 std::move(body))};
	}

	constexpr auto forStmt() noexcept -> std::optional<ast::Statement>
	{}

	constexpr auto elseStmt() noexcept -> std::optional<ast::ElseStmt>
	{
		if(not lexer().next_is(lexing::TokenTypes::ELSE)) {
			// TODO: return error "expected else kexword"
			return std::nullopt;
		}

		auto start = lexer().peek_and_pop().value().getArea();

		auto body_opt = block();
		if(not body_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto [body, end] = std::move(body_opt.value());

		auto area = lexing::TextArea::combine(start, end);

		return ast::ElseStmt{area, std::move(body)};
	}

	constexpr auto elifStmt() noexcept -> std::optional<ast::ElifStmt>
	{
		if(not lexer().next_is(lexing::TokenTypes::ELIF)) {
			return std::nullopt;
		}

		auto start = lexer().peek_and_pop().value().getArea();

		auto elif_condition_opt = expression();
		if(not elif_condition_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto elif_condition = std::move(elif_condition_opt.value());

		auto body_opt = block();
		if(not body_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto [body, end] = std::move(body_opt.value());

		auto area = lexing::TextArea::combine(start, end);

		return ast::ElifStmt{area,
							 std::move(elif_condition),
							 std::move(body)};
	}

	constexpr auto ifStmt() noexcept -> std::optional<ast::Statement>
	{
		if(not lexer().next_is(lexing::TokenTypes::IF)) {
			return std::nullopt;
		}

		auto start = lexer().peek_and_pop().value().getArea();

		auto condition_opt = expression();
		if(not condition_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto condition = std::move(condition_opt.value());

		auto body_opt = block();
		if(not body_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto [body, end] = std::move(body_opt.value());

		auto area = lexing::TextArea::combine(start, end);

		std::vector<ast::ElifExpr> elifs;
		while(lexer().next_is(lexing::TokenTypes::ELIF)) {
			auto elif_opt = elifStmt();
			if(not elif_opt) {
				// TODO: propagate error
				return std::nullopt;
			}
			elifs.emplace_back(std::move(elif_opt.value()));
			area = lexing::TextArea::combine(start, elifs.back().getArea());
		}

		std::optional<ast::ElseStmt> else_;
		if(lexer().next_is(lexing::TokenTypes::ELSE)) {
			auto else_opt = elseStmt();
			if(not else_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			area = lexing::TextArea::combine(start, else_opt.value().getArea());
			else_ = std::move(else_opt.value());
		}

		return ast::Statement{
			ast::forward<ast::IfStmt>(area,
									  std::move(condition),
									  std::move(body),
									  std::move(elifs),
									  std::move(else_))};
	}



private:
	constexpr auto identifier() noexcept
		-> std::optional<ast::Identifier>
	{
		return static_cast<T*>(this)->identifier();
	}

	constexpr auto expression() noexcept -> std::optional<ast::Expression>
	{
		return static_cast<T*>(this)->expression();
	}

	constexpr auto type() noexcept -> std::optional<ast::Type>
	{
		return static_cast<T*>(this)->type();
	}

	constexpr auto lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}
};

} // namespace parser
