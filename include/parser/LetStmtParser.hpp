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
class LetStmtParser
{
public:
	// parses tuples, literals, lambda expressions and (expressions)
	constexpr auto let() noexcept -> std::optional<ast::Statement>
	{
		if(not let_stmt_lexer().next_is(lexing::TokenTypes::LET)) {
			return std::nullopt;
		}

		// pop the let token and get the area of it as start
		auto start = let_stmt_lexer().peek_and_pop().value().getArea();

		auto id_opt = static_cast<T*>(this)->identifier();
		if(not id_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto id = std::move(id_opt.value());

		// parse optional type annotation
		std::optional<ast::Type> t;
		if(let_stmt_lexer().next_is(lexing::TokenTypes::COLON)) {
			let_stmt_lexer().pop();

			auto type_opt = static_cast<T*>(this)->type();
			if(not type_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			t = std::move(type_opt.value());
		}

		if(not let_stmt_lexer().next_is(lexing::TokenTypes::ASSIGN)) {
			// TODO: return error "expected ="
			return std::nullopt;
		}

		auto rhs_opt = static_cast<T*>(this)->expression();
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

private:
	constexpr auto let_stmt_lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}
};

} // namespace parser
