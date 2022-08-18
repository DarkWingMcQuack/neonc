cmake_minimum_required(VERSION 3.12)

#get options and compiling env
include(cmake/options.cmake)

project(Neon)
project(Neon LANGUAGES CXX)

#set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################
###get external projects
############################
include(cmake/fmt.cmake)
include(cmake/tbb.cmake)
include(cmake/ctre.cmake)
include(cmake/namedtype.cmake)
include(cmake/expected.cmake)


# needed for multithreading
find_package(Threads REQUIRED)


#############################################
## library with all the logic which
## can later be easily linked against test
#############################################
add_library(NeonLib INTERFACE) 

target_sources(NeonLib
  INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/include/lexer/Tokens.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lexer/Lexer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lexer/Regexes.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lexer/TextArea.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/lexer/Tokens.hpp

  
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/common/AreaBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/common/Identifier.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/common/Self.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Addition.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/BitwiseAnd.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/BitwiseOr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/BlockExpr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Boolean.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Division.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Double.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Equal.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/ForExpr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/FunctionCall.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/GreaterEqThen.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/GreaterThen.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/IfExpr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Integer.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LambdaExpr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LessEqThen.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LessThen.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LogicalAnd.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LogicalNot.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/LogicalOr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Multiplication.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/NotEqual.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Remainder.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/String.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/Substraction.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/TupleExpr.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/UnaryMinus.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/expression/UnaryPlus.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/import/DirectImport.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/import/TypeclassImport.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/statement/LetAssignment.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/statement/ReturnStmt.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/statement/WhileStmt.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/toplevel/FunctionDefinition.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/toplevel/Namespace.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/type/LambdaType.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/type/NamedType.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/type/OptionalType.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/type/TupleType.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/type/UnionType.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/ast/Ast.hpp

  ${CMAKE_CURRENT_LIST_DIR}/include/parser/Utils.hpp
)

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(NeonLib fmt-project)
add_dependencies(NeonLib tbb-project)
add_dependencies(NeonLib namedtype-project)
add_dependencies(NeonLib expected-lite-project)
add_dependencies(NeonLib ctre-project)

# make headers available
target_include_directories(NeonLib INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${FMT_INCLUDE_DIR}
  ${NAMEDTYPE_INCLUDE_DIR}
  ${EXPECTED_LITE_INCLUDE_DIR}
  ${CTRE_INCLUDE_DIR}
  tbb
)

#link against libarys
target_link_libraries(NeonLib INTERFACE
  ${CMAKE_THREAD_LIBS_INIT}
  ${FMT_LIBRARY}
  tbb)

###############################
## THE ACTUAL BINARY
###############################
add_executable(neonc ${CMAKE_CURRENT_SOURCE_DIR}/cmd/main.cpp)


target_include_directories(neonc PRIVATE
  NeonLib
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${FMT_INCLUDE_DIR}
  ${NAMEDTYPE_INCLUDE_DIR}
  ${EXPECTED_LITE_INCLUDE_DIR}
  ${CTRE_INCLUDE_DIR}
  tbb
)

#link against libarys
target_link_libraries(neonc PRIVATE
  NeonLib
  ${CMAKE_THREAD_LIBS_INIT}
  ${FMT_LIBRARY}
  tbb)

add_dependencies(neonc NeonLib)


# if (BUILD_TESTS)
#   enable_testing()
#   add_subdirectory(test)
# endif (BUILD_TESTS)

# if (BUILD_BENCHMARKS)
#   add_subdirectory(benchmark)
# endif (BUILD_BENCHMARKS)
