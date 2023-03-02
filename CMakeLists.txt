cmake_minimum_required (VERSION 3.14...3.22.1)
project (ANS_compressor)

INCLUDE(CMakeForceCompiler)
set(CMAKE_C_COMPILER $ENV{CC})
set(CMAKE_CXX_COMPILER $ENV{CXX})

enable_language(CUDA)
set(CMAKE_CUDA_FLAGS_DEBUG "-O0 -g -G -gencode arch=compute_70,code=sm_70")
set(CMAKE_CUDA_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_subdirectory (third_party EXCLUDE_FROM_ALL)
add_subdirectory (src)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()

include(CMakeLists_format.cmake)
include(CMakeLists_cppcheck.cmake)
