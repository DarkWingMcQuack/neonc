#pragma once

#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <vector>

namespace ast {


/**
 * @class TupleExpr
 * @brief Represents a tuple of expressions in an Abstract Syntax Tree.
 * @details This class stores a vector of expressions that make up a tuple.
 */
class TupleExpr : public AreaBase
{
public:
    /**
     * @brief Constructor that sets the operation and area of the TupleExpr.
     * @param area Represents the area where the operation is done.
     * @param exprs Represents the expressions that form the tuple.
     */
    constexpr TupleExpr(lexing::TextArea area, std::vector<Expression>&& exprs) noexcept
        : AreaBase(area),
          expressions_(std::move(exprs)) {}

    // Deleting the default constructor and the copy constructor
    constexpr TupleExpr() noexcept = delete;
    constexpr TupleExpr(const TupleExpr&) noexcept = delete;

    // Providing the default move constructor and move assignment operator
    constexpr TupleExpr(TupleExpr&&) noexcept = default;
    constexpr auto operator=(TupleExpr&&) noexcept -> TupleExpr& = default;

    // Deleting the copy assignment operator
    constexpr auto operator=(const TupleExpr&) noexcept -> TupleExpr& = delete;

    /**
     * @brief Equality comparison operator. Checks if two TupleExpr objects are equal.
     * @param other The TupleExpr object to compare with.
     * @return true if the TupleExpr objects are equal, false otherwise.
     */
    constexpr auto operator==(const TupleExpr& other) const noexcept -> bool
    {
        return expressions_ == other.expressions_;
    }

    /**
     * @brief Returns a const reference to the vector of expressions that form the tuple.
     * @return const reference to expressions_.
     */
    constexpr auto getExpressions() const noexcept -> const std::vector<Expression>&
    {
        return expressions_;
    }

    /**
     * @brief Returns a non-const reference to the vector of expressions that form the tuple.
     * @return non-const reference to expressions_.
     */
    constexpr auto getExpressions() noexcept -> std::vector<Expression>&
    {
        return expressions_;
    }

private:
    std::vector<Expression> expressions_; ///< Vector of expressions that form the tuple
};

} // namespace ast
