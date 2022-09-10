include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

ExternalProject_Add(ctre-project
  PREFIX deps/ctre
  DOWNLOAD_NAME ctre-3.7.tar.gz
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  URL https://github.com/hanickadot/compile-time-regular-expressions/archive/refs/tags/v3.7.tar.gz
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  CMAKE_ARGS ${CMAKE_ARGS}
  # Overwtire build and install commands to force Release build on MSVC.
  BUILD_COMMAND ""
  INSTALL_COMMAND cmake -E copy_directory <SOURCE_DIR>/single-header <INSTALL_DIR>/include/ctre
  DOWNLOAD_NO_PROGRESS ON
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )


ExternalProject_Get_Property(ctre-project INSTALL_DIR)
add_library(ctre STATIC IMPORTED)

set(CTRE_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${CTRE_INCLUDE_DIR})  # Must exist.
set_property(TARGET ctre PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CTRE_INCLUDE_DIR})

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
