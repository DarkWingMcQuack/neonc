include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DJSON_BuildTests=FALSE
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

ExternalProject_Add(nlohmann-project
  PREFIX deps/nlohmann
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  CMAKE_ARGS ${CMAKE_ARGS}
  PATCH_COMMAND cmake -E make_directory <SOURCE_DIR>/win32-deps/include
  BUILD_COMMAND cmake --build <BINARY_DIR> --config Release
  UPDATE_COMMAND ""
  INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install
  )

ExternalProject_Get_Property(nlohmann-project INSTALL_DIR)
set(NLOHMANN_INCLUDE_DIR ${INSTALL_DIR}/include)

unset(INSTALL_DIR)
unset(CMAKE_ARGS)
