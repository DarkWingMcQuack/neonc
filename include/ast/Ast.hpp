#pragma once

#include <ast/Forward.hpp>
#include <variant>

#include <ast/common/Identifier.hpp>
#include <ast/expression/Boolean.hpp>
#include <ast/expression/Double.hpp>
#include <ast/expression/Integer.hpp>
#include <ast/expression/SelfExpr.hpp>
#include <ast/expression/String.hpp>
#include <ast/import/DirectImport.hpp>
#include <ast/type/NamedType.hpp>
#include <ast/type/SelfType.hpp>
#include <common/Traits.hpp>

namespace ast {

class UnionType;
class TupleType;
class OptionalType;
class LambdaType;

using Type = std::variant<NamedType,
                          SelfType,
                          Forward<UnionType>,
                          Forward<TupleType>,
                          Forward<OptionalType>,
                          Forward<LambdaType>>;

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
class MemberAccess;

using Expression = std::variant<Identifier,
                                Integer,
                                Double,
                                Boolean,
                                String,
                                SelfExpr,
                                Forward<IfExpr>,
                                Forward<FunctionCall>,
                                Forward<LambdaExpr>,
                                Forward<TupleExpr>,
                                Forward<BlockExpr>,
                                Forward<ForExpr>,
                                Forward<Addition>,
                                Forward<Substraction>,
                                Forward<Multiplication>,
                                Forward<Division>,
                                Forward<Remainder>,
                                Forward<LogicalOr>,
                                Forward<LogicalAnd>,
                                Forward<BitwiseOr>,
                                Forward<BitwiseAnd>,
                                Forward<LessThen>,
                                Forward<LessEqThen>,
                                Forward<GreaterThen>,
                                Forward<GreaterEqThen>,
                                Forward<Equal>,
                                Forward<NotEqual>,
                                Forward<UnaryMinus>,
                                Forward<UnaryPlus>,
                                Forward<LogicalNot>,
                                Forward<MemberAccess>>;

class TypeclassImport;
using Import = std::variant<DirectImport,
                            Forward<TypeclassImport>>;


class LetAssignment;
class WhileStmt;
class ForStmt;
class IfStmt;

using Statement = std::variant<Import,
                               Expression,
                               Forward<LetAssignment>,
                               Forward<WhileStmt>,
                               Forward<IfStmt>,
                               Forward<ForStmt>>;

using FunctionStatement = std::variant<Expression,
                                       Import,
                                       Forward<LetAssignment>,
                                       Forward<WhileStmt>>;

class Namespace;
class FunctionDefinition;
class TypeDefinition;
class TypeclassDefinition;

using ToplevelElement = std::variant<Import,
                                     Forward<FunctionDefinition>,
                                     Forward<Namespace>,
                                     Forward<TypeDefinition>,
                                     Forward<TypeclassDefinition>,
                                     Forward<LetAssignment>>;


template<class T>
constexpr auto getTextArea(const T& ast_element) noexcept
    -> lexing::TextArea
{
    // clang-format off
    constexpr bool is_variant = common::is_specialization_of<std::variant, T>::value;
	constexpr bool is_forward = common::is_specialization_of<Forward, T>::value;
    // clang-format on

    if constexpr(is_forward) {
        return ast_element->getArea();
    } else if constexpr(not is_variant) {
        return ast_element.getArea();
    } else {

        return std::visit(
            [](const auto& e) {
                constexpr bool is_forward = common::is_specialization_of<Forward, std::decay_t<decltype(e)>>::value;

                if constexpr(is_forward) {
                    return e->getArea();
                } else {
                    return e.getArea();
                }
            },
            ast_element);
    }
}


template<class T>
constexpr auto setTextArea(T& ast_element, lexing::TextArea new_area) noexcept
{
    // clang-format off
    constexpr bool is_variant = common::is_specialization_of<std::variant, T>::value;
	constexpr bool is_forward = common::is_specialization_of<Forward, T>::value;
    // clang-format on

    if constexpr(is_forward) {
        ast_element->setArea(new_area);
    } else if constexpr(not is_variant) {
        ast_element.setArea(new_area);
    } else {

        return std::visit(
            [&](auto& e) {
                constexpr bool is_forward = common::is_specialization_of<Forward, std::decay_t<decltype(e)>>::value;

                if constexpr(is_forward) {
                    e->setArea(new_area);
                } else {
                    e.setArea(new_area);
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

#include <ast/expression/BinaryOperation.hpp>
#include <ast/expression/BlockExpr.hpp>
#include <ast/expression/ForExpr.hpp>
#include <ast/expression/FunctionCall.hpp>
#include <ast/expression/IfExpr.hpp>
#include <ast/expression/LambdaExpr.hpp>
#include <ast/expression/TupleExpr.hpp>
#include <ast/expression/UnaryOperation.hpp>

#include <ast/statement/ForStmt.hpp>
#include <ast/statement/IfStmt.hpp>
#include <ast/statement/LetAssignment.hpp>
#include <ast/statement/WhileStmt.hpp>

#include <ast/import/TypeclassImport.hpp>
