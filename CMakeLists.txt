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
include(cmake/flags.cmake)


# needed for multithreading
find_package(Threads REQUIRED)


#############################################
## library with all the logic which
## can later be easily linked against test
#############################################
file(GLOB NEONLIB_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp)

add_library(NeonLib INTERFACE) 

target_sources(NeonLib INTERFACE ${NEONLIB_HEADER_FILES})

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

set_flags(neonc)
setup_linker(neonc)


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


if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif (BUILD_TESTS)

# if (BUILD_BENCHMARKS)
#   add_subdirectory(benchmark)
# endif (BUILD_BENCHMARKS)
