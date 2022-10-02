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
class ForElementParser
{
public:
	constexpr auto for_element() noexcept -> std::optional<ast::ForElement>
	{
		auto id_opt = static_cast<T*>(this)->identifier();
		if(not id_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}
		auto id = std::move(id_opt.value());

		if(for_element_lexer().next_is(lexing::TokenTypes::L_ARROW)) {
			return monadic_for_element(std::move(id));
		}

		if(for_element_lexer().next_is(lexing::TokenTypes::ASSIGN)) {
			return let_for_element(std::move(id));
		}

		// return "expected <- or =" error
		return std::nullopt;
	}

	constexpr auto monadic_for_element(ast::Identifier&& id) noexcept
		-> std::optional<ast::ForMonadicElement>
	{
		// sanity check
		if(not for_element_lexer().next_is(lexing::TokenTypes::L_ARROW)) {
			return std::nullopt;
		}
		for_element_lexer().pop();

		auto rhs_opt = static_cast<T*>(this)->expression();
		if(not rhs_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}

		auto rhs = std::move(rhs_opt.value());
		auto start = id.getArea();
		auto end = ast::getTextArea(rhs);
		auto area = lexing::TextArea::combine(start, end);

		return ast::ForMonadicElement{std::move(area),
									  std::move(id),
									  std::move(rhs)};
	}

	constexpr auto let_for_element(ast::Identifier&& id) noexcept
		-> std::optional<ast::ForLetElement>
	{
		// sanity check
		if(not for_element_lexer().next_is(lexing::TokenTypes::ASSIGN)) {
			return std::nullopt;
		}
		for_element_lexer().pop();

		auto rhs_opt = static_cast<T*>(this)->expression();
		if(not rhs_opt.has_value()) {
			// TODO: propagate error
			return std::nullopt;
		}

		auto rhs = std::move(rhs_opt.value());
		auto start = id.getArea();
		auto end = ast::getTextArea(rhs);
		auto area = lexing::TextArea::combine(start, end);

		return ast::ForLetElement{std::move(area),
								  std::move(id),
								  std::move(rhs)};
	}

private:
	constexpr auto for_element_lexer() noexcept -> lexing::Lexer&
	{
		return static_cast<T*>(this)->lexer_;
	}
};

} // namespace parser
