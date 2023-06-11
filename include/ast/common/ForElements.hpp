#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

/**
 * @class ForLetElement
 *
 * @brief A class representing a 'let' element inside a 'for' loop in the abstract syntax tree.
 *
 * This class is used to represent a 'let' element inside a 'for' loop in an abstract syntax tree (AST).
 * It holds the name of the variable and the expression that is assigned to it.
 *
 * The class inherits from AreaBase which is used to keep track of the area in the source code
 * that the represented element spans.
 */
class ForLetElement : public AreaBase
{
public:
    /**
     * @brief Constructs a 'let' element representation with the provided area, name, and expression.
     *
     * @param area The area in the source code that the 'let' element spans.
     * @param name The name of the variable.
     * @param rhs The expression that is assigned to the variable.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr ForLetElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    // The class cannot be default-constructed.
    constexpr ForLetElement() noexcept = delete;

    // The class cannot be copy-constructed.
    constexpr ForLetElement(const ForLetElement&) noexcept = delete;

    // Default move constructor.
    constexpr ForLetElement(ForLetElement&&) noexcept = default;

    // The class cannot be copy-assigned.
    constexpr auto operator=(const ForLetElement&) noexcept -> ForLetElement& = delete;

    // Default move assignment operator.
    constexpr auto operator=(ForLetElement&&) noexcept -> ForLetElement& = default;

    /**
     * @brief Checks equality between this 'let' element and another one.
     *
     * @param other The other 'let' element to compare with.
     * @return True if the 'let' elements are equal, otherwise false.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto operator==(const ForLetElement& other) const noexcept -> bool
    {
        return name_ == other.name_ and rhs_ == other.rhs_;
    }

    /**
     * @brief Gets a constant reference to the name of the variable in the 'let' element.
     *
     * @return A constant reference to the name of the variable.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }

    /**
     * @brief Gets a mutable reference to the name of the variable in the 'let' element.
     *
     * @return A mutable reference to the name of the variable.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    /**
     * @brief Gets a constant reference to the right-hand side expression in the 'let' element.
     *
     * @return A constant reference to the right-hand side expression.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }

    /**
     * @brief Gets a mutable reference to the right-hand side expression in the 'let' element.
     *
     * @return A mutable reference to the right-hand side expression.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_; ///< The name of the variable in the 'let' element.
    Expression rhs_; ///< The right-hand side expression in the 'let' element.
};


/**
 * @class ForMonadicElement
 *
 * @brief A class representing a monadic element inside a 'for' loop in the abstract syntax tree.
 *
 * This class is used to represent a monadic element inside a 'for' loop in an abstract syntax tree (AST).
 * Similar to the 'let' element, it holds the name of the variable and the expression.
 *
 * The class inherits from AreaBase which is used to keep track of the area in the source code
 * that the represented element spans.
 */
class ForMonadicElement : public AreaBase
{
public:
    /**
     * @brief Constructs a monadic element representation with the provided area, name, and expression.
     *
     * @param area The area in the source code that the monadic element spans.
     * @param name The name of the variable.
     * @param rhs The expression.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr ForMonadicElement(lexing::TextArea area, Identifier&& name, Expression&& rhs) noexcept
        : AreaBase(area),
          name_(std::move(name)),
          rhs_(std::move(rhs)) {}

    // The class cannot be default-constructed.
    constexpr ForMonadicElement() noexcept = delete;

    // The class cannot be copy-constructed.
    constexpr ForMonadicElement(const ForMonadicElement&) noexcept = delete;

    // Default move constructor.
    constexpr ForMonadicElement(ForMonadicElement&&) noexcept = default;

    // The class cannot be copy-assigned.
    constexpr auto operator=(const ForMonadicElement&) noexcept -> ForMonadicElement& = delete;

    // Default move assignment operator.
    constexpr auto operator=(ForMonadicElement&&) noexcept -> ForMonadicElement& = default;

    /**
     * @brief Checks equality between this monadic element and another one.
     *
     * @param other The other monadic element to compare with.
     * @return True if the monadic elements are equal, otherwise false.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto operator==(const ForMonadicElement& other) const noexcept -> bool
    {
        return name_ == other.name_ and rhs_ == other.rhs_;
    }

    /**
     * @brief Gets a constant reference to the name of the variable in the monadic element.
     *
     * @return A constant reference to the name of the variable.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getName() const noexcept -> const Identifier&
    {
        return name_;
    }

    /**
     * @brief Gets a mutable reference to the name of the variable in the monadic element.
     *
     * @return A mutable reference to the name of the variable.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getName() noexcept -> Identifier&
    {
        return name_;
    }

    /**
     * @brief Gets a constant reference to the right-hand side expression in the monadic element.
     *
     * @return A constant reference to the right-hand side expression.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }

    /**
     * @brief Gets a mutable reference to the right-hand side expression in the monadic element.
     *
     * @return A mutable reference to the right-hand side expression.
     *
     * @note This function is marked noexcept, indicating that it doesn't throw exceptions.
     */
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Identifier name_; ///< The name of the variable in the monadic element.
    Expression rhs_; ///< The right-hand side expression in the monadic element.
};

} // namespace ast
