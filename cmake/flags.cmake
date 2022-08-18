############################
###Compiler flags
############################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}\
          -pipe\
          -Wall\
          -Werror=format-security\
          -ferror-limit=2\
          -O0\
          -g3\
          -fno-optimize-sibling-calls\
          -fno-omit-frame-pointer\
          -Wfloat-equal\
          -Wpointer-arith\
          -Wformat-nonliteral\
          -Winit-self\
          -ggdb")

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}\
          -pipe\
          -Wall\
          -O3\
          -flto\
          -march=native")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}\
          -pipe\
          -Wall\
          -fmax-errors=2\
          -Werror=format-security\
          -O0\
          -g3\
          -Wfloat-equal\
          -Wpointer-arith\
          -Wformat-nonliteral\
          -Winit-self\
          -ggdb")

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}\
          -pipe\
          -Wall\
          -flto\
          -fconcepts-diagnostics-depth=5\
          -O3\
          -fipa-pta\
          -fdevirtualize-at-ltrans\
          -march=native")
endif()
