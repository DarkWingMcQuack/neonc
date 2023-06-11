#pragma once

#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

/**
 * @class AreaBase
 * @brief Base class for representing an area in the abstract syntax tree (AST).
 * @details This class provides a base abstraction for representing an area in the AST. It encapsulates a TextArea object
 * that represents the area where a specific element is located in the source code.
 *
 */
class AreaBase
{
public:
    /**
     * @brief Construct a new AreaBase object.
     * @details Constructs an AreaBase object with the given TextArea representing the area in the source code.
     *
     * @param area The TextArea object representing the area in the source code.
     */
    constexpr AreaBase(lexing::TextArea area) noexcept
        : area_(area) {}

    /**
     * @brief Virtual destructor for AreaBase.
     * @details Ensures proper destruction of derived classes.
     */
    constexpr virtual ~AreaBase() noexcept = default;

    /**
     * @brief Getter for the area of the element.
     * @details Returns the TextArea object representing the area where the element is located in the source code.
     *
     * @return lexing::TextArea The area of the element.
     */
    [[nodiscard]] constexpr auto getArea() const noexcept -> lexing::TextArea
    {
        return area_;
    }

    /**
     * @brief Setter for the area of the element.
     * @details Sets the TextArea object representing the area where the element is located in the source code.
     *
     * @param area The TextArea object representing the area of the element.
     */
    constexpr auto setArea(lexing::TextArea area) noexcept -> void
    {
        area_ = std::move(area);
    }

private:
    lexing::TextArea area_; ///< The area where the element is located in the source code.
};

} // namespace ast
