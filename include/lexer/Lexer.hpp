#pragma once

#include <common/Error.hpp>
#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
#include <exception>
#include <expected>
#include <fmt/core.h>
#include <lexer/Regexes.hpp>
#include <lexer/TextArea.hpp>
#include <lexer/Tokens.hpp>
#include <optional>
#include <queue>
#include <string_view>
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
    /**
     * @brief Construct a new Lexer object from a string view.
     *
     * This constructor takes a string view as input and initializes the Lexer
     * with the content provided. The string view is expected to contain the
     * source code that the Lexer will tokenize.
     *
     * @param content The source code to be tokenized as a string view.
     *
     * @note The Lexer does not own the string view and does not modify its content.
     *
     * Usage Example:
     * @code
     *     auto lexer = lexing::Lexer("let x = 10");
     * @endcode
     */
    constexpr Lexer(std::string_view content) noexcept
        : content_(content) {}

    constexpr Lexer(Lexer&&) noexcept = default;
    constexpr auto operator=(Lexer&&) noexcept -> Lexer& = default;
    constexpr Lexer(const Lexer&) noexcept = delete;
    constexpr auto operator=(const Lexer&) noexcept -> Lexer& = delete;

    /**
     * @brief Peeks at the next token from the lexer without removing it from the internal buffer.
     *
     * This is the specialized version of the function for N=1. It only peeks at the next token.
     *
     * @tparam N The number of tokens to peek. This template argument defaults to 1.
     *
     * @return On success, a std::expected object containing the next token. On failure, it contains an error object.
     *
     * @pre The template argument N must be equal to 1.
     */
    template<auto N = 1>
    constexpr auto peek() noexcept -> std::expected<Token, common::error::Error>
        requires(N == 1) // This method only supports peeking one token.
    {
        // If there are no tokens currently in the internal buffer...
        if(lexed_.empty()) {
            // Attempt to lex the next token.
            auto next = lexNext();

            // If lexing failed, return the failure.
            if(not next.has_value()) {
                return next;
            }

            // If lexing was successful, move the token into the internal buffer.
            lexed_.emplace_back(std::move(next.value()));
        }

        // Return the first token in the internal buffer.
        return lexed_.front();
    }

    /**
     * @brief Peeks at the next N tokens from the lexer without removing them from the internal buffer.
     *
     * This is the specialized version of the function for N>1. It peeks at the next N tokens and
     * returns them as a std::array.
     *
     * @tparam N The number of tokens to peek.
     *
     * @return On success, a std::expected object containing the next N tokens as a std::array. On failure, it contains an error object.
     *
     * @pre The template argument N must be greater than 1.
     */
    template<auto N>
    constexpr auto peek() noexcept -> std::expected<std::array<Token, N>, common::error::Error>
        requires(N > 1) // This method supports peeking multiple tokens.
    {
        // Continue lexing tokens until the buffer has at least N tokens.
        while(lexed_.size() < N) {
            // Lex the next token.
            auto next = lexNext();

            // If lexing failed, return the failure.
            if(not next.has_value()) {
                return std::unexpected(std::move(next.error()));
            }

            // If lexing was successful, move the token into the internal buffer.
            lexed_.emplace_back(std::move(next.value()));
        }

        // Return the first N tokens in the internal buffer.
        return array_from_lexed<N>();
    }

    /**
     * @brief Checks if the type of the next token matches the provided type.
     *
     * This function calls the `peek` function to get the next token and then checks if its type
     * matches the input type.
     *
     * @param type The expected type of the next token.
     *
     * @return true if the next token's type matches the input type, false otherwise.
     *         Also returns false if there is no next token (i.e., if `peek` fails).
     */
    constexpr auto next_is(TokenTypes type) noexcept -> bool
    {
        // Use the peek method to get the next token.
        if(auto result = peek()) {
            // If peeking was successful, compare the type of the token with the input type.
            return result.value().getType() == type;
        }

        // If peeking failed or there are no more tokens, return false.
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


    /**
     * @brief Removes the first N tokens from the lexer's internal token buffer.
     *
     * This function attempts to remove the first N tokens from the `lexed_` internal token buffer.
     * If fewer than N tokens are present, it removes all tokens.
     *
     * @tparam N The number of tokens to remove. This template argument defaults to 1.
     *
     * @pre The template argument N must be of type `size_t` (or any other unsigned integer type).
     *      Negative values are not allowed.
     */
    template<auto N = 1ul>
    constexpr auto pop() noexcept -> void
    {
        // Compute the iterator pointing to the element following the last token to be erased.
        // If there are fewer than N tokens, this iterator points to the end of the token buffer.
        auto end_of_range = lexed_.size() > N ? std::begin(lexed_) + N : std::end(lexed_);

        // Erase the first N tokens from the internal token buffer.
        lexed_.erase(std::begin(lexed_), end_of_range);
    }

    /**
     * @brief Peeks at the first N tokens in the lexer's internal token buffer and then removes them.
     *
     * This function calls the `peek<N>` function to retrieve the first N tokens and then calls
     * `pop<N>` to remove those tokens from the buffer. It returns the tokens that were peeked at
     * before they were removed.
     *
     * @tparam N The number of tokens to peek at and remove. This template argument defaults to 1.
     *
     * @return On success, a std::expected object containing the first N tokens in the buffer before removal.
     *         On failure, it contains an error object. The error object can be a lexing error from `peek<N>` or
     *         an error indicating there were fewer than N tokens in the buffer.
     */
    template<auto N = 1>
    constexpr auto peek_and_pop() noexcept
    {
        // Retrieve the first N tokens from the internal buffer.
        auto res = peek<N>();

        // Remove those N tokens from the buffer.
        pop<N>();

        // Return the tokens that were peeked at.
        return res;
    }

    constexpr auto get_current_position() const noexcept
        -> TextArea
    {
        return TextArea{position_, position_};
    }

    /**
     * @brief Checks if the next token is of a specific type and if so, pops it from the lexer.
     *
     * This method checks if the next token in the lexer matches the provided type.
     * If it does, the token is removed from the lexer and returned. If the next token
     * does not match the provided type or if there are no more tokens in the lexer,
     * the method returns std::nullopt.
     *
     * @param type The type of token to check for and potentially pop.
     *
     * @return A std::optional containing the popped token if the next token matches
     *         the provided type. Otherwise, std::nullopt is returned.
     *
     * @note The token is removed from the lexer if it matches the provided type.
     *
     * @exception No exceptions are thrown as it's a noexcept function.
     *
     * Usage Example:
     * @code
     *     auto lexer = lexing::Lexer("let x = 10");
     *     auto token = lexer.pop_next_is(lexing::TokenTypes::LET);
     *     if (token.has_value()) {
     *         std::cout << "Token found and popped: " << token.value().toString() << '\n';
     *     }
     * @endcode
     */
    constexpr auto pop_next_is(TokenTypes type) noexcept -> std::optional<Token>
    {
        // Check if the next token matches the provided type
        if(next_is(type)) {
            // If it does, pop the token and return it
            return peek_and_pop().value();
        }

        // If there is no next token or if it doesn't match the provided type, return std::nullopt
        return std::nullopt;
    }

    /**
     * @brief Checks if the next token's type matches one of the expected types.
     *
     * This function peeks at the next token and pops it from the lexer's internal token buffer.
     * Then, it checks if the token's type matches any of the types provided as template parameters.
     * If the token's type matches, the function returns the token; otherwise, it returns an
     * `UnexpectedToken` error.
     *
     * @tparam types The expected types of the next token. Can be one or more token type constants.
     *
     * @return On success, a std::expected object containing the next token.
     *         On failure, a std::expected object containing an `UnexpectedToken` error. This error
     *         includes the unexpected token's type, its location in the source code, and the expected
     *         types.
     *
     * @note The function pops the next token from the buffer regardless of whether its type matches
     *       the expected types.
     */
    template<lexing::TokenTypes... types>
    constexpr auto expect() noexcept -> std::expected<lexing::Token, common::error::Error>
    {
        using common::error::Error;
        using common::error::UnexpectedToken;
        using lexing::Token;

        // Peek at and pop the next token from the buffer.
        return peek_and_pop()
            .and_then(
                [](auto token) -> std::expected<Token, Error> {
                    // If the token's type does not match any of the expected types...
                    if(((types != token.getType()) and ...)) {
                        // Create an UnexpectedToken error.
                        UnexpectedToken error{token.getType(),
                                              token.getArea(),
                                              types...};

                        // Return the error.
                        return std::unexpected(std::move(error));
                    }

                    // If the token's type matches one of the expected types, return the token.
                    return token;
                });
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
