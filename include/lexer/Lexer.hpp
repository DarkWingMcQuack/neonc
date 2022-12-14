#pragma once

#include <common/Error.hpp>
#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
#include <expected>
#include <fmt/core.h>
#include <lexer/Regexes.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <optional>
#include <queue>
#include <string_view>
#include <expected>
#include <vector>

namespace lexing {

class Lexer
{
    // helper functions to construct an std::array of size N inplace when the Type T is not default constructable
    template<size_t... Inds>
    auto array_from_lexed_impl(std::integer_sequence<size_t, Inds...>) -> std::array<Token, sizeof...(Inds)>
    {
        return {lexed_[Inds]...};
    }

    template<auto N>
    auto array_from_lexed() -> std::array<Token, N>
    {
        return array_from_lexed_impl(std::make_index_sequence<N>{});
    }

public:
    constexpr Lexer(std::string_view content) noexcept
        : content_(content) {}

    constexpr Lexer(Lexer&&) noexcept = default;
    constexpr auto operator=(Lexer&&) noexcept -> Lexer& = default;
    constexpr Lexer(const Lexer&) noexcept = delete;
    constexpr auto operator=(const Lexer&) noexcept -> Lexer& = delete;

    template<auto N = 1>
    constexpr auto peek() noexcept -> std::expected<Token, common::error::Error>
        requires(N == 1)
    {
        if(lexed_.empty()) {
            auto next = lexNext();
            if(not next.has_value()) {
                return next;
            }

            lexed_.emplace_back(std::move(next.value()));
        }

        return lexed_.back();
    }

    template<auto N>
    constexpr auto peek() noexcept -> std::optional<std::array<Token, N>>
        requires(N > 1)
    {
        while(lexed_.size() < N) {

            auto next = lexNext();

            if(not next.has_value()) {
                return std::nullopt;
            }

            lexed_.emplace_back(std::move(next.value()));
        }


        return array_from_lexed<N>();
    }

    // checks if the next token is of the given type
    constexpr auto next_is(TokenTypes type) noexcept -> bool
    {
        if(auto result = peek()) {
            return result.value().getType() == type;
        }
        return false;
    }

    // checks if the next token is of the given type
    constexpr auto next_is_operator() noexcept -> bool
    {
        if(auto result = peek()) {
            return result.value().isOperator();
        }
        return false;
    }

    // checks if the next token is of the given type
    constexpr auto next_is_prefix_operator() noexcept -> bool
    {
        if(auto result = peek()) {
            return result.value().isPrefixOperator();
        }
        return false;
    }

    // checks if the next token is of the given type
    constexpr auto next_is_postfix_operator() noexcept -> bool
    {
        if(auto result = peek()) {
            return result.value().isPostfixOperator();
        }
        return false;
    }

    constexpr auto next_is_infix_operator() noexcept -> bool
    {
        if(auto result = peek()) {
            return result.value().isInfixOperator();
        }
        return false;
    }

    // checks if the next token is of the given type and pops it if so
    // if not return false
    constexpr auto pop_next_is(TokenTypes type) noexcept -> bool
    {
        if(next_is(type)) {
            pop();
            return true;
        }

        return false;
    }

    constexpr auto next_area() noexcept -> std::optional<TextArea>
    {
        if(auto result = peek()) {
            return result.value().getArea();
        }
        return std::nullopt;
    }

    template<auto N = 1ul>
    constexpr auto pop() noexcept -> void
    {
        for(std::size_t i = 0; i < N; i++) {
            lexed_.pop_back();
        }
    }

    constexpr auto pop_token(TokenTypes type) noexcept -> void
    {
        while(not lexed_.empty() and lexed_.back().getType() == type) {
            lexed_.pop_back();
            peek();
        }
    }

    constexpr auto pop_ws() noexcept -> void
    {
        pop_token(TokenTypes::WHITESPACE);
    }

    template<auto N = 1>
    constexpr auto peek_and_pop() noexcept
    {
        auto res = peek<N>();
        pop<N>();
        return res;
    }

    constexpr auto get_current_position() const noexcept
        -> TextArea
    {
        return TextArea{position_, position_};
    }

private:
    constexpr auto lexNext() noexcept -> std::expected<Token, common::error::Error>
    {
        const auto start = position_;

        if(content_.empty()) {
            return Token{TokenTypes::END_OF_FILE, start, content_};
        }

        // skip whitespace
        if(auto match = ws_re(content_)) {
            [[maybe_unused]] auto _ = moveForward(match.size());
            return lexNext();
        }

        if(auto match = nl_re(content_)) {
            position_ += match.size();
            auto value = moveForward(match.size());
            return Token{TokenTypes::NEWLINE, start, value};
        }


        if(std::isalpha(content_.front())) {
            if(let_re(content_)) {
                auto value = moveForward(3);
                return Token{TokenTypes::LET, start, value};
            }
            if(true_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::TRUE, start, value};
            }
            if(false_re(content_)) {
                auto value = moveForward(5);
                return Token{TokenTypes::FALSE, start, value};
            }
            if(fun_re(content_)) {
                auto value = moveForward(3);
                return Token{TokenTypes::FUN, start, value};
            }
            if(typeclass_re(content_)) {
                auto value = moveForward(9);
                return Token{TokenTypes::TYPECLASS, start, value};
            }
            if(type_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::TYPE, start, value};
            }
            if(if_re(content_)) {
                auto value = moveForward(2);
                return Token{TokenTypes::IF, start, value};
            }
            if(elif_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::ELIF, start, value};
            }
            if(else_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::ELSE, start, value};
            }
            if(for_re(content_)) {
                auto value = moveForward(3);
                return Token{TokenTypes::FOR, start, value};
            }
            if(while_re(content_)) {
                auto value = moveForward(5);
                return Token{TokenTypes::WHILE, start, value};
            }
            if(self_value_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::SELF_VALUE, start, value};
            }
            if(self_type_re(content_)) {
                auto value = moveForward(4);
                return Token{TokenTypes::SELF_TYPE, start, value};
            }
        }

        // check _ before identifier to avoid lexing _ as identifier
        if(underscore_re(content_)) {
            auto value = moveForward(1);
            return Token{TokenTypes::UNDERSCORE, start, value};
        }

        if(auto match = identifier_re(content_)) {
            auto value = moveForward(match.size());
            return Token{TokenTypes::IDENTIFIER, start, value};
        }

        // needs to be checked before integers
        if(auto match = double_re(content_)) {
            auto value = moveForward(match.size());
            return Token{TokenTypes::DOUBLE, start, value};
        }

        if(auto match = integer_re(content_)) {
            auto value = moveForward(match.size());
            return Token{TokenTypes::INTEGER, start, value};
        }

        if(content_.starts_with("::")) {
            auto value = moveForward(2);
            return Token{TokenTypes::COLON_COLON, start, value};
        }

        if(content_.starts_with(':')) {
            auto value = moveForward(1);
            return Token{TokenTypes::COLON, start, value};
        }

        if(content_.starts_with('\"')) {
            return lexStandardString();
        }

        if(content_.starts_with('.')) {
            auto value = moveForward(1);
            return Token{TokenTypes::DOT, start, value};
        }

        if(content_.starts_with(',')) {
            auto value = moveForward(1);
            return Token{TokenTypes::COMMA, start, value};
        }

        if(content_.starts_with('?')) {
            auto value = moveForward(1);
            return Token{TokenTypes::QUESTIONMARK, start, value};
        }


        // this needs to be checked before -
        if(content_.starts_with("->")) {
            auto value = moveForward(2);
            return Token{TokenTypes::R_ARROW, start, value};
        }

        // this needs to be checked before <
        if(content_.starts_with("<-")) {
            auto value = moveForward(2);
            return Token{TokenTypes::L_ARROW, start, value};
        }

        // this needs to be checked before <
        if(content_.starts_with("<=")) {
            auto value = moveForward(2);
            return Token{TokenTypes::LE, start, value};
        }

        // this needs to be checked before >
        if(content_.starts_with(">=")) {
            auto value = moveForward(2);
            return Token{TokenTypes::GE, start, value};
        }

        // this needs to be checked before =
        if(content_.starts_with("==")) {
            auto value = moveForward(2);
            return Token{TokenTypes::EQ, start, value};
        }

        // this needs to be checked before !
        if(content_.starts_with("!=")) {
            auto value = moveForward(2);
            return Token{TokenTypes::NEQ, start, value};
        }

        // this needs to be checked before =
        if(content_.starts_with("=>")) {
            auto value = moveForward(2);
            return Token{TokenTypes::LAMBDA_ARROW, start, value};
        }

        // needs to be checked before /
        if(content_.starts_with("//")) {
            auto value = skipToNewLine();
            return Token{TokenTypes::LINE_COMMENT_START, start, value};
        }


        if(content_.starts_with("/")) {
            auto value = moveForward(1);
            return Token{TokenTypes::DIVISION, start, value};
        }

        if(content_.starts_with('<')) {
            auto value = moveForward(1);
            return Token{TokenTypes::LT, start, value};
        }

        if(content_.starts_with('>')) {
            auto value = moveForward(1);
            return Token{TokenTypes::GT, start, value};
        }

        if(content_.starts_with('=')) {
            auto value = moveForward(1);
            return Token{TokenTypes::ASSIGN, start, value};
        }

        if(content_.starts_with('+')) {
            auto value = moveForward(1);
            return Token{TokenTypes::PLUS, start, value};
        }

        if(content_.starts_with('-')) {
            auto value = moveForward(1);
            return Token{TokenTypes::MINUS, start, value};
        }

        if(content_.starts_with('!')) {
            auto value = moveForward(1);
            return Token{TokenTypes::LOGICAL_NOT, start, value};
        }

        if(content_.starts_with('*')) {
            auto value = moveForward(1);
            return Token{TokenTypes::ASTERIX, start, value};
        }

        if(content_.starts_with('%')) {
            auto value = moveForward(1);
            return Token{TokenTypes::PERCENT, start, value};
        }

        if(content_.starts_with('(')) {
            auto value = moveForward(1);
            return Token{TokenTypes::L_PARANTHESIS, start, value};
        }

        if(content_.starts_with(')')) {
            auto value = moveForward(1);
            return Token{TokenTypes::R_PARANTHESIS, start, value};
        }

        if(content_.starts_with('{')) {
            auto value = moveForward(1);
            return Token{TokenTypes::L_BRACKET, start, value};
        }

        if(content_.starts_with('}')) {
            auto value = moveForward(1);
            return Token{TokenTypes::R_BRACKET, start, value};
        }

        if(content_.starts_with("||")) {
            auto value = moveForward(2);
            return Token{TokenTypes::LOGICAL_OR, start, value};
        }
        if(content_.starts_with("&&")) {
            auto value = moveForward(2);
            return Token{TokenTypes::LOGICAL_AND, start, value};
        }

        if(content_.starts_with("|")) {
            auto value = moveForward(1);
            return Token{TokenTypes::BITWISE_OR, start, value};
        }
        if(content_.starts_with("&")) {
            auto value = moveForward(1);
            return Token{TokenTypes::BITWISE_AND, start, value};
        }
        if(content_.starts_with(";")) {
            auto value = moveForward(1);
            return Token{TokenTypes::SEMICOLON, start, value};
        }

        return std::unexpected(make_unknown_token_error());
    }

    constexpr auto make_unknown_token_error() noexcept
        -> common::error::Error
    {
        TextArea area{position_, position_};
        return common::error::UnknownToken{std::move(area)};
    }

    constexpr auto moveForward(std::size_t n) noexcept -> std::string_view
    {
        n = std::min(content_.size(), n);
        auto split_off = content_.substr(0, n);
        content_ = content_.substr(n);
        position_ += n;

        return split_off;
    }

    constexpr auto skipToNewLine() noexcept -> std::string_view
    {
        auto pos = content_.find_first_of("\n");
        pos = std::min(pos, content_.size());

        auto value = content_.substr(0, pos);
        content_ = content_.substr(pos);
        position_ += pos;

        return value;
    }

    // TODO: implement f-strings
    constexpr auto lexStandardString() noexcept -> std::expected<Token, common::error::Error>
    {
        // start at i = to skip first "
        bool last_was_slash = false;
        std::size_t i = 1;
        for(; i < content_.length(); i++) {
            if(content_[i] == '\"' and not last_was_slash) {
                break;
            }

            last_was_slash = content_[i] == '\\' and not last_was_slash;
        }

        // TODO: here there should be a 'missing closing "'-error be reported somehow
        if(content_[i] != '\"') [[unlikely]] {
            TextArea area{position_, position_};
            common::error::Error error = common::error::UnclosedString{std::move(area)};
            return std::unexpected(std::move(error));
        }

        const auto start = position_;
        const auto value = moveForward(i + 1);

        return Token{TokenTypes::STANDARD_STRING, start, value};
    }

private:
    std::string_view content_;
    std::uint64_t position_;
    std::size_t line_ = 0;
    std::size_t column_ = 0;
    std::vector<Token> lexed_;
};

} // namespace lexing
