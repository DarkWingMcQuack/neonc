#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for parsing if expressions
template<class T>
class ExpressionPrattParser
{
public:
	constexpr auto expression() noexcept -> std::optional<ast::Expression>
	{
		return expression_bp(0);
	}

private:
	constexpr auto expression_bp(std::uint32_t min_bp) noexcept -> std::optional<ast::Expression>
	{
		auto lhs_opt = simple_expr();
		if(not lhs_opt.has_value()) {
			return std::nullopt;
		}
		auto lhs = std::move(lhs_opt.value());

		while(expr_lexer().next_is_operator()) {
			const auto op = expr_lexer().peek().value();

			auto bp_opt = infix_binding_power(op.getType());
			if(not bp_opt.has_value()) {
				return std::nullopt;
			}
			const auto [l_bp, r_bp] = std::move(bp_opt.value());

			if(l_bp < min_bp) {
				break;
			}

			expr_lexer().pop();

			auto rhs_opt = expression_bp(r_bp);
			if(not rhs_opt.has_value()) {
				return std::nullopt;
			}
			auto rhs = std::move(rhs_opt.value());

			lhs_opt = build_expr(std::move(lhs), op, std::move(rhs));
			if(not lhs_opt.has_value()) {
				return std::nullopt;
			}
			lhs = std::move(lhs_opt.value());
		}

		return std::move(lhs);
	}

	constexpr auto build_expr(ast::Expression&& lhs, lexing::Token op, ast::Expression&& rhs) noexcept
		-> std::optional<ast::Expression>
	{
		auto area = lexing::TextArea::combine(ast::getTextArea(lhs),
											  ast::getTextArea(rhs));

		switch(op.getType()) {
		case lexing::TokenTypes::PLUS:
			return ast::Expression{ast::forward<ast::Addition>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::MINUS:
			return ast::Expression{ast::forward<ast::Substraction>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::ASTERIX:
			return ast::Expression{ast::forward<ast::Multiplication>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::DIVISION:
			return ast::Expression{ast::forward<ast::Division>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::PERCENT:
			return ast::Expression{ast::forward<ast::Remainder>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::LOGICAL_OR:
			return ast::Expression{ast::forward<ast::LogicalOr>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::LOGICAL_AND:
			return ast::Expression{ast::forward<ast::LogicalAnd>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::BITWISE_OR:
			return ast::Expression{ast::forward<ast::BitwiseOr>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::BITWISE_AND:
			return ast::Expression{ast::forward<ast::BitwiseAnd>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::LT:
			return ast::Expression{ast::forward<ast::LessThen>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::LE:
			return ast::Expression{ast::forward<ast::LessEqThen>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::GT:
			return ast::Expression{ast::forward<ast::GreaterThen>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::GE:
			return ast::Expression{ast::forward<ast::GreaterEqThen>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::EQ:
			return ast::Expression{ast::forward<ast::Equal>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::NEQ:
			return ast::Expression{ast::forward<ast::NotEqual>(area, std::move(lhs), std::move(rhs))};
		case lexing::TokenTypes::DOT:
			return ast::Expression{ast::forward<ast::MemberAccess>(area, std::move(lhs), std::move(rhs))};
		default:
			return std::nullopt;
		}
	}

	constexpr static auto infix_binding_power(lexing::TokenTypes t) noexcept
		-> std::optional<std::pair<std::uint32_t, std::uint32_t>>
	{
		switch(t) {
		case lexing::TokenTypes::LOGICAL_OR:
			return std::pair{1, 2};

		case lexing::TokenTypes::LOGICAL_AND:
			return std::pair{3, 4};

		case lexing::TokenTypes::BITWISE_OR:
			return std::pair{5, 6};

		case lexing::TokenTypes::BITWISE_AND:
			return std::pair{7, 8};

		case lexing::TokenTypes::EQ:
		case lexing::TokenTypes::NEQ:
			return std::pair{9, 10};

		case lexing::TokenTypes::LT:
		case lexing::TokenTypes::LE:
		case lexing::TokenTypes::GT:
		case lexing::TokenTypes::GE:
			return std::pair{11, 12};

		case lexing::TokenTypes::MINUS:
		case lexing::TokenTypes::PLUS:
			return std::pair{13, 14};

		case lexing::TokenTypes::ASTERIX:
		case lexing::TokenTypes::DIVISION:
		case lexing::TokenTypes::PERCENT:
			return std::pair{15, 16};

		default:
			return std::nullopt;
		}
	}

	constexpr static auto prefix_binding_power(lexing::TokenTypes t) noexcept
		-> std::optional<std::uint32_t>
	{
		switch(t) {
		case lexing::TokenTypes::PLUS:
		case lexing::TokenTypes::MINUS:
		case lexing::TokenTypes::LOGICAL_NOT:
			return 17;
		default:
			return std::nullopt;
		}
	}

private:
	constexpr auto expr_lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}

	constexpr auto simple_expr() noexcept -> std::optional<ast::Expression>
	{
		return static_cast<T*>(this)->simple_expression();
	}
};
} // namespace parser
