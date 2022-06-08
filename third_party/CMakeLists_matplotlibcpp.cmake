cmake_minimum_required(VERSION 2.8.2)

project(matplotlibcpp-download NONE)

include(ExternalProject)
ExternalProject_Add(matplotlibcpp
  GIT_REPOSITORY    https://github.com/Cryoris/matplotlib-cpp
  GIT_TAG           master
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/third_party/matplotlibcpp-src"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/third_party/matplotlibcpp-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
ExternalProject_Get_Property(matplotlibcpp SOURCE_DIR)
set(matplotlib_INCLUDE ${SOURCE_DIR})