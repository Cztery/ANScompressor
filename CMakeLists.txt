cmake_minimum_required (VERSION 3.14...3.22.1)
project (ANS_compressor)

INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER($ENV{CC} GNU)
CMAKE_FORCE_CXX_COMPILER($ENV{CXX} GNU)

set(CMAKE_CXX_STANDARD 17)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_subdirectory (third_party EXCLUDE_FROM_ALL)
add_subdirectory (src)

set (MAKE_DIRECTORY bin)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()

include(CMakeLists_format.cmake)
