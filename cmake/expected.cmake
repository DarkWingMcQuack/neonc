include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DEXPECTED_OPT_BUILD_TESTS=OFF
  -DEXPECTED_OPT_BUILD_EXAMPLES=OFF)

ExternalProject_Add(expected-project
  PREFIX deps/expected
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  GIT_REPOSITORY https://github.com/TartanLlama/expected
  CMAKE_ARGS ${CMAKE_ARGS}
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  UPDATE_COMMAND ""
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  DOWNLOAD_NO_PROGRESS ON
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

ExternalProject_Get_Property(expected-project INSTALL_DIR)
set(EXPECTED_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
