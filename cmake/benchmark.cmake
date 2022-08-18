include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DBENCHMARK_ENABLE_TESTING=OFF
  -DBENCHMARK_ENABLE_EXCEPTIONS=OFF
  -DBENCHMARK_ENABLE_LTO=ON
  -DBENCHMARK_INSTALL_DOCS=OFF
  -DBENCHMARK_ENABLE_DOXYGEN=OFF
  -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
  -DBENCHMARK_ENABLE_BENCHMARK_TESTS=OFF)

ExternalProject_Add(benchmark-project
  PREFIX deps/benchmark
  DOWNLOAD_NAME benchmark-1.6.0.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/google/benchmark/archive/refs/tags/v1.6.0.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )


ExternalProject_Get_Property(benchmark-project INSTALL_DIR)
add_library(benchmark STATIC IMPORTED)
set(BENCHMARK_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX})
set(BENCHMARK_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${BENCHMARK_INCLUDE_DIR})  # Must exist.
set_property(TARGET benchmark PROPERTY IMPORTED_LOCATION ${BENCHMARK_LIBRARY})
set_property(TARGET benchmark PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BENCHMARK_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
