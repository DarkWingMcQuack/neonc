#pragma once

#include <compare>
#include <cstdint>
#include <ctre/ctre.hpp>
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
public:
    constexpr Lexer(std::string_view content) noexcept
        : content_(content) {}

    // TODO: this should not return an std::optional, but an std::expected
    // which makes it possible to return errormessages from the lexer, like "unkown token"
    constexpr auto peek() noexcept -> std::optional<Token>
    {
        if(!peek_.has_value()) {
            peek_ = lexNext();
        }

        return peek_;
    }

    constexpr auto pop() noexcept -> void
    {
        peek_ = std::nullopt;
    }


private:
    constexpr auto lexNext() noexcept -> std::optional<Token>
    {
        const auto start = position_;

        if(content_.empty()) {
            return Token{TokenTypes::END_OF_FILE, start, content_};
        }

        if(auto match = ws_re(content_)) {
            auto value = moveForward(match.size());
            return Token{TokenTypes::WHITESPACE, start, value};
        }

        if(auto match = nl_re(content_)) {
            position_ += match.size();
            auto value = content_.substr(0, match.size());
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

            auto match = identifier_re(content_);
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

        return std::nullopt;
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
    // TODO: implement escape sequences
    // FIXME: does currently not work
    constexpr auto lexStandardString() noexcept -> std::optional<Token>
    {
        // start at i = to skip first "
        std::size_t i = 1;
        for(; i < content_.length(); i++) {
            if(content_[i] == '\"') {
                break;
            }
        }

        // TODO: here there should be a 'missing closing "'-error be reported somehow
        if(content_[i] != '\"') [[unlikely]] {
            return std::nullopt;
        }

        const auto start = position_;
        const auto value = moveForward(i+1);

        return Token{TokenTypes::STANDARD_STRING, start, value};
    }

private:
    std::string_view content_;
    std::uint64_t position_;
    std::size_t line_ = 0;
    std::size_t column_ = 0;
    std::optional<Token> peek_;
};

} // namespace lexing
