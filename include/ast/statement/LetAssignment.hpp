#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <ast/common/Identifier.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

/**
 * @class LetAssignment
 * @brief Represents a let assignment statement in the Abstract Syntax Tree for the Neon programming language.
 * @details This class stores an Identifier for the variable name, an optional Type for the variable type, and an Expression for the right-hand-side of the assignment.
 */
class LetAssignment : public AreaBase
{
public:
    /**
     * @brief Constructor that sets the area, variable name, type, and right-hand-side of the assignment.
     * @param area Represents the area where the assignment is defined.
     * @param name Represents the name of the variable being assigned.
     * @param type Represents the type of the variable (optional).
     * @param rhs Represents the expression that is the right-hand-side of the assignment.
     */
    constexpr LetAssignment(lexing::TextArea area,
                            Identifier&& name,
                            std::optional<Type>&& type,
                            Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          type_(std::move(type)),
          rhs_(std::move(rhs)) {}

    // Deleting the default constructor and the copy constructor to prevent object slicing
    constexpr LetAssignment() noexcept = delete;
    constexpr LetAssignment(const LetAssignment&) noexcept = delete;

    // Providing the default move constructor and move assignment operator
    constexpr LetAssignment(LetAssignment&&) noexcept = default;
    constexpr auto operator=(LetAssignment&&) noexcept -> LetAssignment& = default;

    // Deleting the copy assignment operator to prevent object slicing
    constexpr auto operator=(const LetAssignment&) noexcept -> LetAssignment& = delete;

    /**
     * @brief Equality comparison operator. Checks if two LetAssignment objects are equal.
     * @param other The LetAssignment object to compare with.
     * @return true if the LetAssignment objects are equal, false otherwise.
     */
    constexpr auto operator==(const LetAssignment& other) const noexcept -> bool
    {
        return name_ == other.name_
            and type_ == other.type_
            and rhs_ == other.rhs_;
    }

    /**
     * @brief Returns a const reference to the variable name.
     * @return const reference to name_.
     */
    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }

    /**
     * @brief Returns a non-const reference to the variable name.
     * @return non-const reference to name_.
     */
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    /**
     * @brief Checks if the assignment has a type specified.
     * @return true if a type is specified, false otherwise.
     */
    constexpr auto hasType() const noexcept -> bool
    {
        return type_.has_value();
    }

    /**
     * @brief Returns a const reference to the optional type of the variable.
     * @return const reference to type_.
     */
    constexpr auto getType() const noexcept -> const std::optional<Type>&
    {
        return type_;
    }

    /**
     * @brief Returns a non-const reference to the optional type of the variable.
     * @return non-const reference to type_.
     */
    constexpr auto getType() noexcept -> std::optional<Type>&
    {
        return type_;
    }

    /**
     * @brief Sets the type of the variable.
     * @param type The type to set.
     */
    constexpr auto setType(ast::Type&& type) noexcept
    {
        type_ = std::move(type);
    }

    /**
     * @brief Returns a const reference to the expression on the right-hand-side of the assignment.
     * @return const reference to rhs_.
     */
    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }

    /**
     * @brief Returns a non-const reference to the expression on the right-hand-side of the assignment.
     * @return non-const reference to rhs_.
     */
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_; ///< Name of the variable
    std::optional<Type> type_; ///< Optional type of the variable
    Expression rhs_; ///< Expression on the right-hand-side of the assignment
};

} // namespace ast
