cmake_minimum_required (VERSION 3.16.3)
project (ANS_compressor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory (third_party)
add_subdirectory (src)

set_property(DIRECTORY src PROPERTY CXX_STANDARD 20)

set (MAKE_DIRECTORY bin)

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
    set (MAKE_DIRECTORY tests_out)
    add_test(NAME ansTest COMMAND ansTest)
endif()

include(CMakeLists_format.cmake)