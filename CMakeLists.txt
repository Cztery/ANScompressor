cmake_minimum_required (VERSION 3.16.3)
project (ANS_compressor)

set(CMAKE_CXX_STANDARD 20)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

add_compile_options(
       -Wall -Werror -Wno-error=maybe-uninitialized -Wpedantic
       $<$<CONFIG:RELEASE>:-Ofast>
       $<$<CONFIG:DEBUG>:-O0>
       $<$<CONFIG:DEBUG>:-ggdb3>
)

add_subdirectory (third_party EXCLUDE_FROM_ALL)
add_subdirectory (src)

set_property(DIRECTORY src PROPERTY CXX_STANDARD 20)

set (MAKE_DIRECTORY bin)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
endif()

include(CMakeLists_format.cmake)