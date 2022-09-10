cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../cmake/gtest.cmake)

add_executable(unit_tests
  lexer/LexerTest.cpp
  parser/ParserTest.cpp
  parser/NamedTypeParsingTest.cpp
  parser/SelfTypeParsingTest.cpp
  parser/OptionalTypeParsingTest.cpp
  parser/LambdaTypeParsingTest.cpp
  parser/IdentifierParserTest.cpp
  main.cpp
)

target_link_libraries(unit_tests LINK_PUBLIC
  gtest
  fmt
)


target_include_directories(
  unit_tests PUBLIC
  gtest
  ${FMT_INCLUDE_DIR}
  ${NAMEDTYPE_INCLUDE_DIR}
  ${EXPECTED_LITE_INCLUDE_DIR}
  ${CTRE_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(
  NAME unit_tests
  COMMAND
  ${CMAKE_BINARY_DIR}/test/unit_tests
)


add_dependencies(unit_tests gtest-project)

