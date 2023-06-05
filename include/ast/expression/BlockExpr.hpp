
#pragma once

#include <ast/Ast.hpp>
#include <memory>
#include <vector>

namespace ast {

/**
 * @class BlockExpr
 * @brief Represents a block of code in the Abstract Syntax Tree for the Neon programming language.
 * @details This class stores a vector of statements that make up the body of a block, along with a return expression.
 * @example
 * {
 *   let a = 10;
 *   let b = a * a;
 *   => b * 2
 * }
 */
class BlockExpr : public AreaBase
{
public:
    /**
     * @brief Constructor that sets the area, body, and return expression of the BlockExpr.
     * @param area Represents the area where the block is defined.
     * @param body Represents the statements that form the body of the block.
     * @param ret_expr Represents the return expression of the block.
     */
    constexpr BlockExpr(lexing::TextArea area,
                        std::vector<Statement>&& body,
                        Expression&& ret_expr) noexcept
        : AreaBase(area),
          body_(std::move(body)),
          ret_expr_(std::move(ret_expr)) {}

    // Deleting the default constructor and the copy constructor
    constexpr BlockExpr() noexcept = delete;
    constexpr BlockExpr(const BlockExpr&) noexcept = delete;

    // Providing the default move constructor and move assignment operator
    constexpr BlockExpr(BlockExpr&&) noexcept = default;
    constexpr auto operator=(BlockExpr&&) noexcept -> BlockExpr& = default;

    // Deleting the copy assignment operator to prevent
    constexpr auto operator=(const BlockExpr&) noexcept -> BlockExpr& = delete;

    /**
     * @brief Equality comparison operator. Checks if two BlockExpr objects are equal.
     * @param other The BlockExpr object to compare with.
     * @return true if the BlockExpr objects are equal, false otherwise.
     */
    constexpr auto operator==(const BlockExpr& other) const noexcept -> bool
    {
        return body_ == other.body_ and ret_expr_ == other.ret_expr_;
    }

    /**
     * @brief Returns a const reference to the vector of statements that form the body of the block.
     * @return const reference to body_.
     */
    constexpr auto getBody() const noexcept -> const std::vector<Statement>&
    {
        return body_;
    }

    /**
     * @brief Returns a non-const reference to the vector of statements that form the body of the block.
     * @return non-const reference to body_.
     */
    constexpr auto getBody() noexcept -> std::vector<Statement>&
    {
        return body_;
    }

    /**
     * @brief Returns a const reference to the return expression of the block.
     * @return const reference to ret_expr_.
     */
    constexpr auto getReturnExpression() const noexcept -> const Expression&
    {
        return ret_expr_;
    }

    /**
     * @brief Returns a non-const reference to the return expression of the block.
     * @return non-const reference to ret_expr_.
     */
    constexpr auto getReturnExpression() noexcept -> Expression&
    {
        return ret_expr_;
    }

private:
    std::vector<Statement> body_; ///< Vector of statements that form the body of the block
    Expression ret_expr_; ///< Return expression of the block
};

} // namespace ast
