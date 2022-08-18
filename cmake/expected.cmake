include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DEXPECTED_LITE_OPT_BUILD_TESTS=OFF
  -DEXPECTED_LITE_OPT_BUILD_EXAMPLES=OFF)

ExternalProject_Add(expected-lite-project
  PREFIX deps/expected-lite
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  GIT_REPOSITORY https://github.com/martinmoene/expected-lite
  CMAKE_ARGS ${CMAKE_ARGS}
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  UPDATE_COMMAND ""
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

ExternalProject_Get_Property(expected-lite-project INSTALL_DIR)
set(EXPECTED_LITE_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
