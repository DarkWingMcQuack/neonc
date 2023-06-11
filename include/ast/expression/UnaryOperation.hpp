#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

/**
 * @class UnaryOperation
 * @brief This is a base class representing a unary operation in an Abstract Syntax Tree.
 * @details Unary operations are operations with only one operand. They are inherited by specific unary operation classes.
 */
class UnaryOperation : public AreaBase
{
public:
    /**
     * @brief Constructor that sets the operation and area of the UnaryOperation.
     * @param area Represents the area where the operation is done.
     * @param rhs Represents the operand of the unary operation.
     */
    constexpr UnaryOperation(lexing::TextArea area, Expression&& rhs) noexcept
        : AreaBase(area),
          rhs_(std::move(rhs)) {}

    // Deleting the default constructor and the copy constructor to prevent object slicing
    constexpr UnaryOperation() noexcept = delete;
    constexpr UnaryOperation(const UnaryOperation&) noexcept = delete;

    // Providing the default move constructor and move assignment operator
    constexpr UnaryOperation(UnaryOperation&&) noexcept = default;
    constexpr auto operator=(UnaryOperation&&) noexcept -> UnaryOperation& = default;

    // Deleting the copy assignment operator to prevent object slicing
    constexpr auto operator=(const UnaryOperation&) noexcept -> UnaryOperation& = delete;

    /**
     * @brief Pure virtual destructor. UnaryOperation is an abstract base class and cannot be instantiated.
     */
    constexpr virtual ~UnaryOperation() noexcept = 0;

    /**
     * @brief Returns a const reference to the right hand side operand.
     * @return const reference to rhs_.
     */
    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }

    /**
     * @brief Returns a non-const reference to the right hand side operand.
     * @return non-const reference to rhs_.
     */
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

    /**
     * @brief Equality comparison operator. Checks if two UnaryOperation objects are equal.
     * @details Only UnaryOperations of the same derived type can be compared.
     * @param lhs Left-hand-side UnaryOperation object.
     * @param rhs Right-hand-side UnaryOperation object.
     * @return true if lhs and rhs are equal, false otherwise.
     */
    template<typename T>
        requires std::is_base_of_v<UnaryOperation, T>
    friend constexpr auto operator==(const T& lhs, const T& rhs) noexcept
    {
        return lhs.rhs_ == rhs.rhs_;
    }

private:
    Expression rhs_; ///< Right hand side operand of the unary operation
};

// Providing the default destructor for the UnaryOperation
constexpr UnaryOperation::~UnaryOperation() noexcept = default;


/**
 * @class UnaryPlus
 * @brief Represents a unary plus operation in the Abstract Syntax Tree.
 */
class UnaryPlus : public UnaryOperation
{
    using UnaryOperation::UnaryOperation; // Inherits the constructors from UnaryOperation
    using UnaryOperation::operator=; // Inherits the assignment operators from UnaryOperation
};

/**
 * @class UnaryMinus
 * @brief Represents a unary minus operation in the Abstract Syntax Tree.
 */
class UnaryMinus : public UnaryOperation
{
    using UnaryOperation::UnaryOperation; // Inherits the constructors from UnaryOperation
    using UnaryOperation::operator=; // Inherits the assignment operators from UnaryOperation
};

/**
 * @class LogicalNot
 * @brief Represents a logical NOT operation in the Abstract Syntax Tree.
 */
class LogicalNot : public UnaryOperation
{
    using UnaryOperation::UnaryOperation; // Inherits the constructors from UnaryOperation
    using UnaryOperation::operator=; // Inherits the assignment operators from UnaryOperation
};

} // namespace ast
