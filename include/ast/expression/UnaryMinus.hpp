#include <ast/Ast.hpp>
#include <ast/common/AreaBase.hpp>
#include <lexer/TextArea.hpp>
#include <memory>

namespace ast {

class UnaryMinus : public AreaBase
{
public:
    constexpr UnaryMinus(lexing::TextArea area, Expression&& rhs) noexcept
        : AreaBase(area),
          rhs_(std::move(rhs)) {}

    constexpr auto getRightHandSide() const noexcept -> const Expression&
    {
        return rhs_;
    }
    constexpr auto getRightHandSide() noexcept -> Expression&
    {
        return rhs_;
    }

private:
    Expression rhs_;
};

} // namespace ast
