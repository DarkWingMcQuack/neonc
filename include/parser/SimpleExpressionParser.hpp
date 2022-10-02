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
// 10. block expressions
// 11. TODO: list comprehensions
// 12. TODO: for expressions
template<class T>
class SimpleExpressionParser
{
public:
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

		if(auto result = static_cast<T*>(this)->identifier_or_simple_lambda()) {
		  return std::move(result.value());
		}

		if(auto result = static_cast<T*>(this)->if_expression()) {
			return std::move(result.value());
		}


		// if the expressions starts with a (
		// it is a tuple, or a lambda expression or a grouped expression
		if(simple_expr_lexer().next_is(lexing::TokenTypes::L_PARANTHESIS)) {
			auto start = simple_expr_lexer().peek_and_pop().value().getArea();

			// parse the first expression after the ( because thats what all the three options
			// what it could be have in common
			auto expr_opt = static_cast<T*>(this)->expression();
			if(not expr_opt) {
				// TODO: propagate error
				return std::nullopt;
			}
			auto expr = std::move(expr_opt.value());

			// if the next token is ) then it was a grouped expression or a lambda
			if(simple_expr_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
				// check if it could be a lambda otherwise return the expr itself
				return try_lambda(start, std::move(expr));
			}

			// TODO: if next token is a : then it is a lambda expression

			// if next token is a , then we have (<expr>, which coule be a lambda expression "(a, b) => c" or a tuple (a, b)
			if(simple_expr_lexer().next_is(lexing::TokenTypes::COMMA)) {

				// try parsing a tuple or a lambda expression from the expressions lists
				auto res = tuple_or_lambda_params(start, std::move(expr));

				return std::visit(
					[this, start = start](auto&& r) -> std::optional<ast::Expression> {
						if constexpr(std::same_as<std::nullopt_t, std::decay_t<decltype(r)>>) {
							return std::nullopt;
						} else {
							// r can be a list of parameters or a tuple
							// if it is a list of parameters, it is clear that it is a lambda expression
							// but if it is a tuple, it can still be a lambda expression when it is followed by a =>
							return try_lambda(start, std::move(r));
						}
					},
					std::move(res));
			}
		}

		if(simple_expr_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
			return block_expression();
		}

		// return expectedn blablabla error
		return std::nullopt;
	}

private:
	constexpr auto integer() noexcept -> std::optional<ast::Integer>
	{
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::INTEGER)) {
			return std::nullopt;
		}

		auto token = simple_expr_lexer().peek_and_pop().value();
		auto value = parse_int_unsafe(token.getValue());
		return ast::Integer{token.getArea(), value};
	}

	constexpr auto duoble() noexcept -> std::optional<ast::Double>
	{
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::DOUBLE)) {
			return std::nullopt;
		}

		auto token = simple_expr_lexer().peek_and_pop().value();
		auto value = parse_double_unsafe(token.getValue());
		return ast::Double{token.getArea(), value};
	}

	constexpr auto boolean() noexcept -> std::optional<ast::Boolean>
	{
		if(simple_expr_lexer().next_is(lexing::TokenTypes::TRUE)) {
			auto area = simple_expr_lexer().peek_and_pop().value().getArea();
			return ast::Boolean(area, true);
		}

		if(simple_expr_lexer().next_is(lexing::TokenTypes::FALSE)) {
			auto area = simple_expr_lexer().peek_and_pop().value().getArea();
			return ast::Boolean(area, false);
		}

		return std::nullopt;
	}

	constexpr auto self_value() noexcept -> std::optional<ast::SelfExpr>
	{
		if(simple_expr_lexer().next_is(lexing::TokenTypes::SELF_VALUE)) {
			auto area = simple_expr_lexer().peek_and_pop().value().getArea();
			return ast::SelfExpr{area};
		}

		return std::nullopt;
	}

	constexpr auto string() noexcept -> std::optional<ast::String>
	{
		if(simple_expr_lexer().next_is(lexing::TokenTypes::STANDARD_STRING)) {
			auto token = simple_expr_lexer().peek_and_pop().value();
			return ast::String{token.getArea(), token.getValue()};
		}

		return std::nullopt;
	}

	constexpr auto block_expression() noexcept -> std::optional<ast::BlockExpr>
	{
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::L_BRACKET)) {
			// TODO: return error "expected {"
			return std::nullopt;
		}
		// pop the { and get its area
		auto start = simple_expr_lexer().peek_and_pop().value().getArea();

		std::vector<ast::Statement> stmts;
		while(not simple_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			auto stmt_opt = static_cast<T*>(this)->statement();
			if(not stmt_opt.has_value()) {
				// TODO: propagate error
				return std::nullopt;
			}
			stmts.emplace_back(std::move(stmt_opt.value()));

			if(not(simple_expr_lexer().next_is(lexing::TokenTypes::SEMICOLON) or simple_expr_lexer().next_is(lexing::TokenTypes::NEWLINE))) {
				// TODO: return error "expected newline or ;"
				return std::nullopt;
			}
			simple_expr_lexer().pop();
		}

		if(not simple_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			// TODO: return error "a block expression needs to return an expression with =>expr"
			return std::nullopt;
		}
		simple_expr_lexer().pop();

		auto expr_opt = static_cast<T*>(this)->expression();
		if(not expr_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto expr = std::move(expr_opt.value());

		if(not simple_expr_lexer().next_is(lexing::TokenTypes::R_BRACKET)) {
			// TODO: return error: "expected } after block return statment"
			return std::nullopt;
		}

		auto end = simple_expr_lexer().peek_and_pop().value().getArea();
		auto area = lexing::TextArea::combine(start, end);
		return ast::BlockExpr{area, std::move(stmts), std::move(expr)};
	}



	// given a list of exprssions, this method checks if all the expressions are identifiers
	// and if so it converts them to a lits of lambda parameters
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

	// parse a lambda parameter <id> (: <type>)?
	constexpr auto lambda_parameter() noexcept -> std::optional<ast::LambdaParameter>
	{
		auto id_opt = static_cast<T*>(this)->identifier();
		if(not id_opt.has_value()) {
			// TODO: propagte error
			return std::nullopt;
		}
		auto id = std::move(id_opt.value());

		if(simple_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
			simple_expr_lexer().pop();
			auto type_opt = static_cast<T*>(this)->type();
			if(not type_opt.has_value()) {
				// TODO: propagte error
				return std::nullopt;
			}
			auto type = std::move(type_opt.value());

			return ast::LambdaParameter{std::move(id), std::move(type)};
		}
		return ast::LambdaParameter{std::move(id)};
	}

	// parse a lambda parameter which is a simple identifier or "id: <type>" given the id is already parsed
	// and the start is at the colon right before the type
	constexpr auto lambda_parameter(ast::Identifier&& id) noexcept -> std::optional<ast::LambdaParameter>
	{
		if(simple_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
			simple_expr_lexer().pop();
			auto type_opt = static_cast<T*>(this)->type();
			if(not type_opt.has_value()) {
				// TODO: propagte error
				return std::nullopt;
			}
			auto type = std::move(type_opt.value());

			return ast::LambdaParameter{std::move(id), std::move(type)};
		}
		return ast::LambdaParameter{std::move(id)};
	}

	// when parsing a tuple and we have a list of already parsed expressions and then at a point the next expression is <expr>:<type>
	// it is known that the tuple wasnt realy a tuple but a lambda parameter list
	// this method starts right at the first colon
	// it extracts all the already parsed expressions, checks if they are identifiers and if to assignes the type after the first colon
	// to the last parsed identifier
	// then it continues to parse the list of lambda parameters
	constexpr auto lambda_parameters_from_starting_tuple(std::vector<ast::Expression>&& parsed) noexcept
		-> std::optional<std::vector<ast::LambdaParameter>>
	{
		// sanity check
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
			// TODO: log something
			return std::nullopt;
		}


		simple_expr_lexer().pop();

		auto type_opt = static_cast<T*>(this)->type();
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

		while(simple_expr_lexer().next_is(lexing::TokenTypes::COMMA)) {
			simple_expr_lexer().pop();
			auto param_opt = lambda_parameter();
			if(not param_opt.has_value()) {
				return std::nullopt;
			}

			params.emplace_back(std::move(param_opt.value()));
		}

		if(not simple_expr_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
			// TODO: return "expected ) error"
			return std::nullopt;
		}

		return std::make_optional(std::move(params));
	}

	// given (<expr> we try to parse a tuple or a lambda parameter list here
	// when in any case there is a "id : type" pattern in the parsing then it musst be a lambda parameter list
	// if it is just a lits of expressions without type annotation then it musst be a tuple
	constexpr auto tuple_or_lambda_params(lexing::TextArea start, ast::Expression&& first) noexcept
		-> std::variant<ast::TupleExpr,
						std::vector<ast::LambdaParameter>,
						std::nullopt_t>
	{
		std::vector<ast::Expression> expressions;
		expressions.emplace_back(std::move(first));

		while(simple_expr_lexer().next_is(lexing::TokenTypes::COMMA)) {
			simple_expr_lexer().pop();

			auto expr_opt = static_cast<T*>(this)->expression();
			if(not expr_opt) {
				// TODO: propagate error
				return std::nullopt;
			}
			auto expr = std::move(expr_opt.value());
			expressions.emplace_back(std::move(expr));
		}

		if(simple_expr_lexer().next_is(lexing::TokenTypes::COLON)) {
			auto params_opt = lambda_parameters_from_starting_tuple(std::move(expressions));
			if(not params_opt.has_value()) {
				return std::nullopt;
			}
			return std::move(params_opt.value());
		}

		if(not simple_expr_lexer().next_is(lexing::TokenTypes::R_PARANTHESIS)) {
			// TODO: return "expected ) error"
			return std::nullopt;
		}

		auto end = ast::getTextArea(expressions.back());
		auto area = lexing::TextArea::combine(start, end);

		return ast::TupleExpr{area, std::move(expressions)};
	}


	// given a list of lambda parameters such as (a: Int, b, c: Int) it musst be a lambda expression and therefore be followed by a => token
	// consume this token, parse the body and return the lambda expression
	constexpr auto try_lambda(lexing::TextArea start, std::vector<ast::LambdaParameter>&& parameters) noexcept
		-> std::optional<ast::Expression>
	{
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			// TODO: return error "expected =>"
			return std::nullopt;
		}

		// consume =>
		simple_expr_lexer().pop();
		auto body_opt = static_cast<T*>(this)->expression();
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

	// given a parsed tuple expression it could be the case that this is a lambda expression
	// to this be the case the tuple expression needs to be followed by a => token
	// if this is the case it is a lambda expression and all the expressions in the tuple need to be identifiers
	// if the tuple is not followed by a => token we just return the tuple itself, because it is just a tuple and not an lambda expression
	constexpr auto try_lambda(lexing::TextArea start, ast::TupleExpr&& tuple) noexcept
		-> std::optional<ast::Expression>
	{
		// if the next identifier is not a lambda arrow then the expression was realy a tuple expression
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			return ast::Expression{
				ast::forward<ast::TupleExpr>(std::move(tuple))};
		}
		// consume =>
		simple_expr_lexer().pop();

		// parse the body of the lambda expression
		auto body_opt = static_cast<T*>(this)->expression();
		if(not body_opt.has_value()) {
			return std::nullopt;
		}
		auto body = std::move(body_opt.value());

		// calculate the area of the lambda expression
		auto end = ast::getTextArea(body);
		auto area = lexing::TextArea::combine(start, end);

		// move expressions out of the tuple
		auto exprs = std::move(tuple.getExpressions());

		// check if all expressions are identifiers
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

	// given an expression after a (expr)=> we try to parse a lambda here
	// for this to be a valid lambda, the expression needs to be a parameter which means it needs to be an Identifier
	// if that is true we parse the => followed by the lambda body
	// if the expr in (<expresssion>) is not followed by a => it is only a grouped expression and we return the simple expression again
	constexpr auto try_lambda(lexing::TextArea start, ast::Expression&& first) noexcept
		-> std::optional<ast::Expression>
	{
		// if the next identifier is not a lambda arrow then the expression was realy a tuple expression
		if(not simple_expr_lexer().next_is(lexing::TokenTypes::LAMBDA_ARROW)) {
			return std::move(first);
		}

		// consume =>
		simple_expr_lexer().pop();

		auto body_opt = static_cast<T*>(this)->expression();
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
	}

private:
	constexpr auto simple_expr_lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}
};

} // namespace parser
