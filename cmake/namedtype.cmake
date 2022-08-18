include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DENABLE_TEST=OFF)

ExternalProject_Add(namedtype-project
  PREFIX deps/namedtype
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  # GIT_REPOSITORY https://github.com/joboccara/NamedType
  # use this fork until the comparison operator is fixed in the lib
  GIT_REPOSITORY https://github.com/usefulcat/NamedType
  CMAKE_ARGS ${CMAKE_ARGS}
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  UPDATE_COMMAND ""
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

ExternalProject_Get_Property(namedtype-project INSTALL_DIR)
set(NAMEDTYPE_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
