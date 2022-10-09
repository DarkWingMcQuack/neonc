cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../cmake/gtest.cmake)

function (new_test source name)
  add_executable(${name} ${source})
  target_link_libraries(${name} LINK_PUBLIC gtest gtest_main fmt)
  target_include_directories(${name} PUBLIC
	gtest
	${FMT_INCLUDE_DIR}
	${NAMEDTYPE_INCLUDE_DIR}
	${EXPECTED_LITE_INCLUDE_DIR}
	${CTRE_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../include
  )

  set_flags(${name})
  setup_linker(${name})

  include(GoogleTest)
  gtest_discover_tests(${name})
  
  add_dependencies(${name} gtest-project)
endfunction()

new_test(lexer/LexerTest.cpp LexerTest)
new_test(parser/ParserTest.cpp ParserTest)
new_test(parser/NamedTypeParsingTest.cpp NamedTypeParsingTest)
new_test(parser/SelfTypeParsingTest.cpp SelfTypeParsingTest)
new_test(parser/OptionalTypeParsingTest.cpp OptionalTypeParsingTest)
new_test(parser/LambdaTypeParsingTest.cpp LambdaTypeParsingTest)
new_test(parser/TupleTypeParsingTest.cpp TupleTypeParsingTest)
new_test(parser/UnionTypeParsingTest.cpp UnionTypeParsingTest)
new_test(parser/IdentifierParserTest.cpp IdentifierParserTest)
new_test(parser/IdentifierExpressionParserTest.cpp IdentifierExpressionParserTest)
new_test(parser/IntegerExpressionParserTest.cpp IntegerExpressionParserTest)
new_test(parser/StringExpressionParserTest.cpp StringExpressionParserTest)
new_test(parser/BooleanExpressionParserTest.cpp BooleanExpressionParserTest)
new_test(parser/SelfExpressionParserTest.cpp SelfExpressionParserTest)
new_test(parser/IfExpressionParserTest.cpp IfExpressionParserTest)
new_test(parser/LetStatementParserTest.cpp LetStatementParserTest)

new_test(parser/AdditionExprParserTest.cpp AdditionExprParserTest)
new_test(parser/SubstractionExprParserTest.cpp SubstractionExprParserTest)
new_test(parser/MultiplicationExprParserTest.cpp MultiplicationExprParserTest)
new_test(parser/DivisionExprParserTest.cpp DivisionExprParserTest)
new_test(parser/RemainderExprParserTest.cpp RemainderExprParserTest)
new_test(parser/LambdaExprParserTest.cpp LambdaExprParserTest)


