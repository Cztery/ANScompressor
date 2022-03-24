# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
cmake_minimum_required (VERSION 3.16.3)
find_package(Threads)

add_executable(ansTest ansTest.cpp)
add_dependencies(ansTest gtest)

target_include_directories(ansTest PUBLIC ../src)
target_link_libraries(ansTest gtest gtest_main pthread bmplib anslib)

gtest_discover_tests(ansTest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ansTest --output-on-failure DEPENDS ansTest)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
