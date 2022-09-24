#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class Equal : public AreaBase
{
public:
    constexpr Equal(lexing::TextArea area, Expression&& lhs, Expression&& rhs) noexcept
        : AreaBase(area),
          lhs_(std::move(lhs)),
          rhs_(std::move(rhs)) {}

    constexpr auto getLeftHandSide() const noexcept -> const Expression&
    {
        return lhs_;
    }
    constexpr auto getLeftHandSide() noexcept -> Expression&
    {
        return lhs_;
    }

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Expression lhs_;
    Expression rhs_;
};

} // namespace ast
