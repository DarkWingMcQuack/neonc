#pragma once

#include <variant>

#include <ast/common/Identifier.hpp>
#include <ast/common/Self.hpp>
#include <ast/expression/Boolean.hpp>
#include <ast/expression/Double.hpp>
#include <ast/expression/Integer.hpp>
#include <ast/expression/String.hpp>
#include <ast/import/DirectImport.hpp>
#include <ast/type/NamedType.hpp>
#include <common/Traits.hpp>

namespace ast {

class UnionType;
class TupleType;
class OptionalType;
class LambdaType;

using Type = std::variant<NamedType,
                          std::unique_ptr<UnionType>,
                          std::unique_ptr<TupleType>,
                          std::unique_ptr<OptionalType>,
                          std::unique_ptr<LambdaType>>;

class IfExpr;
class FunctionCall;
class LambdaExpr;
class TupleExpr;
class BlockExpr;
class ForExpr;

class Addition;
class Substraction;
class Multiplication;
class Division;
class Remainder;
class LogicalOr;
class LogicalAnd;
class BitwiseOr;
class BitwiseAnd;
class LessThen;
class LessEqThen;
class GreaterThen;
class GreaterEqThen;
class Equal;
class NotEqual;
class UnaryMinus;
class UnaryPlus;
class LogicalNot;

using Expression = std::variant<Identifier,
                                Integer,
                                Double,
                                Boolean,
                                String,
                                Self,
                                std::unique_ptr<IfExpr>,
                                std::unique_ptr<FunctionCall>,
                                std::unique_ptr<LambdaExpr>,
                                std::unique_ptr<TupleExpr>,
                                std::unique_ptr<BlockExpr>,
                                std::unique_ptr<ForExpr>,
                                std::unique_ptr<Addition>,
                                std::unique_ptr<Substraction>,
                                std::unique_ptr<Multiplication>,
                                std::unique_ptr<Division>,
                                std::unique_ptr<Remainder>,
                                std::unique_ptr<LogicalOr>,
                                std::unique_ptr<LogicalAnd>,
                                std::unique_ptr<BitwiseOr>,
                                std::unique_ptr<BitwiseAnd>,
                                std::unique_ptr<LessThen>,
                                std::unique_ptr<LessEqThen>,
                                std::unique_ptr<GreaterThen>,
                                std::unique_ptr<GreaterEqThen>,
                                std::unique_ptr<Equal>,
                                std::unique_ptr<NotEqual>,
                                std::unique_ptr<UnaryMinus>,
                                std::unique_ptr<UnaryPlus>,
                                std::unique_ptr<LogicalNot>>;

class TypeclassImport;
using Import = std::variant<DirectImport,
                            std::unique_ptr<TypeclassImport>>;


class LetAssignment;
class WhileStmt;
class ReturnStmt;

using Statement = std::variant<Import,
                               Expression,
                               std::unique_ptr<LetAssignment>,
                               std::unique_ptr<WhileStmt>>;

using FunctionStatement = std::variant<Expression,
                                       Import,
                                       std::unique_ptr<LetAssignment>,
                                       std::unique_ptr<WhileStmt>,
                                       std::unique_ptr<ReturnStmt>>;

class Namespace;
class FunctionDefinition;
class TypeDefinition;
class TypeclassDefinition;

using ToplevelElement = std::variant<Import,
                                     std::unique_ptr<FunctionDefinition>,
                                     std::unique_ptr<Namespace>,
                                     std::unique_ptr<TypeDefinition>,
                                     std::unique_ptr<TypeclassDefinition>,
                                     std::unique_ptr<LetAssignment>>;


template<class T>
constexpr auto getTextArea(const T& ast_element) noexcept
    -> lexing::TextArea
{
    // clang-format off
    constexpr bool is_variant = common::is_specialization_of<std::variant, T>::value;
	constexpr bool is_unique_ptr = common::is_specialization_of<std::unique_ptr, T>::value;
    // clang-format on

    if constexpr(is_unique_ptr) {
        return ast_element->getArea();
    } else if constexpr(not is_variant) {
        return ast_element.getArea();
    } else {

        return std::visit(
            [](const auto& e) {
                constexpr bool is_unique_ptr = common::is_specialization_of<std::unique_ptr, std::decay_t<decltype(e)>>::value;

                if constexpr(is_unique_ptr) {
                    return e->getArea();
                } else {
                    return e.getArea();
                }
            },
            ast_element);
    }
}

} // namespace ast


#include <ast/type/LambdaType.hpp>
#include <ast/type/NamedType.hpp>
#include <ast/type/OptionalType.hpp>
#include <ast/type/TupleType.hpp>
#include <ast/type/UnionType.hpp>

#include <ast/expression/Addition.hpp>
#include <ast/expression/BitwiseAnd.hpp>
#include <ast/expression/BitwiseOr.hpp>
#include <ast/expression/BlockExpr.hpp>
#include <ast/expression/Division.hpp>
#include <ast/expression/Equal.hpp>
#include <ast/expression/ForExpr.hpp>
#include <ast/expression/FunctionCall.hpp>
#include <ast/expression/GreaterEqThen.hpp>
#include <ast/expression/GreaterThen.hpp>
#include <ast/expression/IfExpr.hpp>
#include <ast/expression/LambdaExpr.hpp>
#include <ast/expression/LessEqThen.hpp>
#include <ast/expression/LessThen.hpp>
#include <ast/expression/LogicalAnd.hpp>
#include <ast/expression/LogicalNot.hpp>
#include <ast/expression/LogicalOr.hpp>
#include <ast/expression/Multiplication.hpp>
#include <ast/expression/NotEqual.hpp>
#include <ast/expression/Remainder.hpp>
#include <ast/expression/Substraction.hpp>
#include <ast/expression/TupleExpr.hpp>
#include <ast/expression/UnaryMinus.hpp>
#include <ast/expression/UnaryPlus.hpp>

#include <ast/statement/LetAssignment.hpp>
#include <ast/statement/WhileStmt.hpp>

#include <ast/import/TypeclassImport.hpp>
