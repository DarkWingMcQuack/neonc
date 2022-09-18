#pragma once

#include <ast/Ast.hpp>
#include <ast/Forward.hpp>
#include <lexer/Lexer.hpp>
#include <parser/TypeParser.hpp>
#include <parser/Utils.hpp>
#include <string_view>

namespace parser {

// crtp class for simple expressions which are:
// 1. tuple
// 2. lambda expressions
// 3. integers
// 4. doubles
// 5. booleans
// 6. strings
// 7. identifiers
// 8. if-else expressions
// 9. grouped expressions which are expresions surrounded by ()
// 10. TODO: list comprehensions
// 11. TODO: for expressions
// 12. TODO: block expressions
template<class T>
class SimpleExpressionParser
{
	// parses tuples, literals, lambda expressions and (expressions)
	constexpr auto simple_expression() noexcept -> std::optional<ast::Expression>
	{
		if(auto result = integer()) {
			return result.value();
		}

		if(auto result = duoble()) {
			return result.value();
		}

		if(auto result = boolean()) {
			return result.value();
		}

		if(auto result = self_value()) {
			return result.value();
		}

		if(auto result = string()) {
			return result.value();
		}

		if(auto result = identifier()) {
			if(lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
				return simple_lambda(std::move(result.value()));
			}
			return result.value();
		}

		if(auto result = if_expression()) {
			return std::move(result.value());
		}

		if(lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
			auto start = lexer().peek_and_pop().value().getArea();
			auto expr_opt = expression();
			if(not expr_opt) {
				// TODO: propagate error
				return std::nullopt;
			}
			auto expr = std::move(expr_opt.value());

			if(lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
				return try_lambda(start, std::move(expr));
			}

			if(lexer().next_is(lexing::TokenTypes::COMMA)) {
				auto res = tuple_or_lambda_params(start, std::move(expr));

				return std::visit(
					[this, start = start](auto&& r) -> std::optional<ast::Expression> {
						if constexpr(std::same_as<std::nullopt_t, std::decay_t<decltype(r)>>) {
							return std::nullopt;
						} else {
							return try_lambda(start, std::move(r));
						}
					},
					std::move(res));
			}

			if(lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
				return block_expression();
			}
		}
	}

private:
	constexpr auto integer() noexcept -> std::optional<ast::Integer>
	{
		if(not lexer().next_is(lexing::TokenTypes::INTEGER)) {
			return std::nullopt;
		}

		auto token = lexer().peek_and_pop().value();
		auto value = parse_int_unsafe(token.getValue());
		return ast::Integer{token.getArea(), value};
	}

	constexpr auto duoble() noexcept -> std::optional<ast::Double>
	{
		if(not lexer().next_is(lexing::TokenTypes::DOUBLE)) {
			return std::nullopt;
		}

		auto token = lexer().peek_and_pop().value();
		auto value = parse_double_unsafe(token.getValue());
		return ast::Double{token.getArea(), value};
	}

	constexpr auto boolean() noexcept -> std::optional<ast::Boolean>
	{
		if(lexer().next_is(lexing::TokenTypes::TRUE)) {
			auto area = lexer().next_area().value();
			return ast::Boolean(area, true);
		}

		if(lexer().next_is(lexing::TokenTypes::FALSE)) {
			auto area = lexer().next_area().value();
			return ast::Boolean(area, false);
		}

		return std::nullopt;
	}

	constexpr auto self_value() noexcept -> std::optional<ast::Self>
	{
		if(lexer().next_is(lexing::TokenTypes::SELF_VALUE)) {
			auto area = lexer().peek_and_pop().value().getArea();
			return ast::Self{area};
		}

		return std::nullopt;
	}

	constexpr auto string() noexcept -> std::optional<ast::String>
	{
		if(lexer().next_is(lexing::TokenTypes::STANDARD_STRING)) {
			auto token = lexer().peek_and_pop().value();
			return ast::String{token.getArea(), token.getValue()};
		}

		return std::nullopt;
	}

	constexpr auto block_expression() noexcept -> std::optional<ast::BlockExpr>
	{
		return std::nullopt;
	}

	constexpr auto elif_expression() noexcept -> std::optional<ast::ElifExpr>
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

		auto block_expr_opt = block_expression();
		if(not block_expr_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto block_expr = std::move(block_expr_opt.value());

		auto end = ast::getTextArea(block_expr);

		return ast::ElifExpr{lexing::TextArea::combine(start, end),
							 std::move(elif_condition),
							 std::move(block_expr)};
	}

	constexpr auto if_expression() noexcept -> std::optional<ast::Expression>
	{
		if(not lexer().next_is(lexing::TokenTypes::IF)) {
			return std::nullopt;
		}

		auto start = lexer().peek_and_pop().value().getArea();

		auto if_condition_opt = expression();
		if(not if_condition_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto if_condition = std::move(if_condition_opt.value());

		auto if_block_expr_opt = block_expression();
		if(not if_block_expr_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto if_block_expr = std::move(if_block_expr_opt.value());

		std::vector<ast::ElifExpr> elifs;
		while(lexer().next_is(lexing::TokenTypes::ELIF)) {
			if(auto elif_expr = elif_expression()) {
				elifs.emplace_back(std::move(elif_expr.value()));
			}
		}

		auto else_block_opt = block_expression();
		if(not else_block_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto else_block = std::move(else_block_opt.value());

		auto end = ast::getTextArea(else_block);

		return ast::Expression{
			ast::forward<ast::IfExpr>(lexing::TextArea::combine(start, end),
									  std::move(if_condition),
									  std::move(if_block_expr),
									  std::move(elifs),
									  std::move(else_block))};
	}

	constexpr auto simple_lambda(ast::Identifier&& parameter) noexcept -> std::optional<ast::Expression>
	{
		if(not lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			return std::nullopt;
		}

		lexer().pop();

		auto expr_opt = expression();
		if(not expr_opt) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto expr = std::move(expr_opt.value());

		auto start = parameter.getArea();
		auto end = ast::getTextArea(expr);
		auto area = lexing::TextArea::combine(start, end);

		std::vector<ast::LambdaParameter> parameters;
		parameters.emplace_back(std::move(parameter));


		return ast::Expression{
			ast::forward<ast::LambdaExpr>(area,
										  std::move(parameters),
										  std::move(expr))};
	}

	constexpr auto exprs_to_parameters(std::vector<ast::Expression>&& parsed) noexcept
		-> std::optional<std::vector<ast::LambdaParameter>>
	{
		for(const auto& elem : parsed) {
			if(not std::holds_alternative<ast::Identifier>(elem)) {
				// TODO: return error "elem" should be an identifier and not an ...
				return std::nullopt;
			}
		}


		std::vector<ast::LambdaParameter> parameters;
		parameters.reserve(parsed.size());

		std::transform(std::make_move_iterator(std::begin(parsed)),
					   std::make_move_iterator(std::end(parsed)),
					   std::back_inserter(parameters),
					   [](auto&& expr) {
						   auto id = std::move(std::get<ast::Identifier>(expr));
						   return ast::LambdaParameter{std::move(id)};
					   });

		return std::move(parameters);
	}

	constexpr auto lambda_parameter() noexcept -> std::optional<ast::LambdaParameter>
	{
		auto id_opt = identifier();
		if(not id_opt.has_value()) {
			// TODO: propagte error
			return std::nullopt;
		}
		auto id = std::move(id_opt.value());

		if(lexer().next_is(lexing::TokenTypes::COLON)) {
			lexer().pop();
			auto type_opt = type();
			if(not type_opt.has_value()) {
				// TODO: propagte error
				return std::nullopt;
			}
			auto type = std::move(type_opt.value());

			return ast::LambdaParameter{std::move(id), std::move(type)};
		}
		return ast::LambdaParameter{std::move(id)};
	}


	constexpr auto lambda_parameters_from_starting_tuple(std::vector<ast::Expression>&& parsed) noexcept
		-> std::optional<std::vector<ast::LambdaParameter>>
	{
		// sanity check
		if(not lexer().next_is(lexing::TokenTypes::COLON)) {
			// TODO: log something
			return std::nullopt;
		}


		lexer().pop();

		auto type_opt = type();
		if(not type_opt.has_value()) {
			// TODO: propagte error
			return std::nullopt;
		}
		auto type = std::move(type_opt.value());

		auto params_opt = exprs_to_parameters(std::move(parsed));
		if(not params_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto params = std::move(params_opt.value());

		params.back().setType(std::move(type));

		while(lexer().next_is(lexing::TokenTypes::COMMA)) {
			lexer().pop();
			auto param_opt = lambda_parameter();
			if(not param_opt.has_value()) {
				return std::nullopt;
			}

			params.emplace_back(std::move(param_opt.value()));
		}

		if(not lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
			// TODO: return "expected ) error"
			return std::nullopt;
		}
	}

	constexpr auto tuple_or_lambda_params(lexing::TextArea start, ast::Expression&& first) noexcept
		-> std::variant<ast::TupleExpr,
						std::vector<ast::LambdaParameter>,
						std::nullopt_t>
	{
		std::vector<ast::Expression> expressions;
		expressions.emplace_back(std::move(first));

		while(lexer().next_is(lexing::TokenTypes::COMMA)) {
			lexer().pop();

			auto expr_opt = expression();
			if(not expr_opt) {
				// TODO: propagate error
				return std::nullopt;
			}
			auto expr = std::move(expr_opt.value());
			expressions.emplace_back(std::move(expr));
		}

		if(lexer().next_is(lexing::TokenTypes::COLON)) {
			auto params_opt = lambda_parameters_from_starting_tuple(std::move(expressions));
			if(not params_opt.has_value()) {
				return std::nullopt;
			}
			return std::move(params_opt.value());
		}

		if(not lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
			// TODO: return "expected ) error"
			return std::nullopt;
		}

		auto end = ast::getTextArea(expressions.back());
		auto area = lexing::TextArea::combine(start, end);

		return ast::TupleExpr{area, std::move(expressions)};
	}

	constexpr auto lambda_parameter(ast::Identifier&& id) noexcept -> std::optional<ast::LambdaParameter>
	{
		if(lexer().next_is(lexing::TokenTypes::COLON)) {
			lexer().pop();
			auto type_opt = type();
			if(not type_opt.has_value()) {
				// TODO: propagte error
				return std::nullopt;
			}
			auto type = std::move(type_opt.value());

			return ast::LambdaParameter{std::move(id), std::move(type)};
		}
		return ast::LambdaParameter{std::move(id)};
	}

	constexpr auto try_lambda(lexing::TextArea start, std::vector<ast::LambdaParameter>&& parameters) noexcept
		-> std::optional<ast::Expression>
	{
		if(not lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			// TODO: return error "expected =>"
			return std::nullopt;
		}

		// consume =>
		lexer().pop();
		auto body_opt = expression();
		if(not body_opt.has_value()) {
			return std::nullopt;
		}
		auto body = std::move(body_opt.value());
		auto end = ast::getTextArea(body);
		auto area = lexing::TextArea::combine(start, end);

		return ast::Expression{
			ast::forward<ast::LambdaExpr>(area,
										  std::move(parameters),
										  std::move(body))};
	};

	constexpr auto try_lambda(lexing::TextArea start, ast::TupleExpr&& tuple) noexcept
		-> std::optional<ast::Expression>
	{
		// if the next identifier is not a lambda arrow then the expression was realy a tuple expression
		if(not lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			return ast::Expression{
				ast::forward<ast::TupleExpr>(std::move(tuple))};
		}
		// consume =>
		lexer().pop();

		auto body_opt = expression();
		if(not body_opt.has_value()) {
			return std::nullopt;
		}
		auto body = std::move(body_opt.value());

		auto end = ast::getTextArea(body);
		auto area = lexing::TextArea::combine(start, end);

		auto exprs = std::move(tuple.getExpressions());

		auto param_opt = exprs_to_parameters(std::move(exprs));
		if(not param_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto parameters = std::move(param_opt.value());

		return ast::Expression{
			ast::forward<ast::LambdaExpr>(area,
										  std::move(parameters),
										  std::move(body))};
	};

	constexpr auto try_lambda(lexing::TextArea start, ast::Expression&& first) noexcept
		-> std::optional<ast::Expression>
	{
		// if the next identifier is not a lambda arrow then the expression was realy a tuple expression
		if(not lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			return std::move(first);
			// consume =>
			lexer().pop();

			auto body_opt = expression();
			if(not body_opt.has_value()) {
				return std::nullopt;
			}
			auto body = std::move(body_opt.value());

			auto end = ast::getTextArea(body);
			auto area = lexing::TextArea::combine(start, end);

			std::vector<ast::Expression> exprs;
			exprs.emplace_back(std::move(first));

			auto param_opt = exprs_to_parameters(std::move(exprs));
			if(not param_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			auto parameters = std::move(param_opt.value());

			return ast::Expression{
				ast::forward<ast::LambdaExpr>(area,
											  std::move(parameters),
											  std::move(body))};
		};
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
