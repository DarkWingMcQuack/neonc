#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for block expressions which are:
// 1. tuple
// 2. lambda expressions
// 3. integers
// 4. doubles
// 5. booleans
// 6. strings
// 7. identifiers
// 8. if-else expressions
// 9. grouped expressions which are expresions surrounded by ()
// 10. block expressions
// 11. TODO: list comprehensions
// 12. TODO: for expressions
template<class T>
class BlockExpressionParser
{
public:
	constexpr auto block_expression() noexcept -> std::optional<ast::BlockExpr>
	{
		if(not block_expr_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
			return std::nullopt;
		}

		// pop the { and get its area
		auto start = block_expr_lexer().peek_and_pop().value().getArea();

		std::vector<ast::Statement> stmts;
		while(not block_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			auto stmt_opt = static_cast<T*>(this)->statement();
			if(not stmt_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			stmts.emplace_back(std::move(stmt_opt.value()));

			// clang-format off
			if(not(block_expr_lexer().pop_next_is(lexing::TokenTypes::SEMICOLON) or
				   block_expr_lexer().pop_next_is(lexing::TokenTypes::NEWLINE))) {
				// TODO: return error "expected newline or ;"
				return std::nullopt;
			}
			// clang-format on
		}

		if(not block_expr_lexer().pop_next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			// TODO: return error "a block expression needs to return an expression with =>expr"
			return std::nullopt;
		}

		auto expr_opt = static_cast<T*>(this)->expression();
		if(not expr_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto expr = std::move(expr_opt.value());

		if(not block_expr_lexer().next_is(lexing::TokenTypes::R_BRACKET)) {
			// TODO: return error: "expected } after block return statment"
			return std::nullopt;
		}

		auto end = block_expr_lexer().peek_and_pop().value().getArea();
		auto area = lexing::TextArea::combine(start, end);
		return ast::BlockExpr{area, std::move(stmts), std::move(expr)};
	}


private:
	constexpr auto block_expr_lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}
};

} // namespace parser
