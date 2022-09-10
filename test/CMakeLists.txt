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
  add_test(
	NAME ${name}
	COMMAND ${CMAKE_BINARY_DIR}/test/${name}
  )

  set_flags(${name})
  setup_linker(${name})
  
  add_dependencies(${name} gtest-project)
endfunction()

new_test(lexer/LexerTest.cpp LexerTest)
new_test(parser/ParserTest.cpp ParserTest)
new_test(parser/NamedTypeParsingTest.cpp NamedTypeParsingTest)
new_test(parser/SelfTypeParsingTest.cpp SelfTypeParsingTest)
new_test(parser/OptionalTypeParsingTest.cpp OptionalTypeParsingTest)
new_test(parser/LambdaTypeParsingTest.cpp LambdaTypeParsingTest)
new_test(parser/IdentifierParserTest.cpp IdentifierParserTest)


