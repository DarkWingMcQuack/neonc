#pragma once

#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <string_view>
#include <utility>
#include <variant>

namespace ast {

/**
 * @class Identifier
 * @brief Represents an identifier in the abstract syntax tree (AST).
 * @details This class extends the AreaBase class and provides an abstraction for identifiers in a Neon.
 * It encapsulates the area where the identifier is located in the source code and the value of the identifier.
 */

class Identifier : public AreaBase
{
public:
    /**
     * @brief Construct a new Identifier object
     * @details Constructs an Identifier object that represents an identifier in the source code.
     *
     * @param area The TextArea object that represents the area where the identifier is located in the source code.
     * @param value The string view that represents the value of the identifier.
     */
    constexpr Identifier(lexing::TextArea area, std::string_view value) noexcept
        : AreaBase{std::move(area)}, value_(value) {}


    /**
     * @brief Equality operator for Identifier.
     * @details Compares this Identifier object with another for equality based on their values.
     *
     * @param other The other Identifier object to compare with.
     * @return true If the value of this Identifier object equals the value of the other.
     * @return false Otherwise.
     */
    constexpr auto operator==(const Identifier& other) const noexcept
        -> bool
    {
        return value_ == other.value_;
    }

    /**
     * @brief Getter for the value of the identifier.
     * @details Returns the value of the Identifier object. The value represents the name of the identifier.
     *
     * @return std::string_view The value of the identifier.
     */
    constexpr auto getValue() const noexcept -> std::string_view
    {
        return value_;
    }

private:
    std::string_view value_; ///< The value of the identifier.
};

} // namespace ast
