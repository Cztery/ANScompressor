# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
cmake_minimum_required (VERSION 3.16.3)
find_package(Threads)

add_executable(bmpTest bmpTest.cpp)
add_dependencies(bmpTest gtest)

target_include_directories(bmpTest PUBLIC ../src)
target_link_libraries(bmpTest gtest gtest_main pthread bmplib anslib)

gtest_discover_tests(bmpTest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} bmpTest --output-on-failure DEPENDS bmpTest)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
