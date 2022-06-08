# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
cmake_minimum_required (VERSION 3.16.3)
find_package(Threads)

add_executable(bmpTest bmpTest.cpp)
add_executable(histTest histTest.cpp)
add_executable(compTest compTest.cpp)

# add_dependencies(bmpTest gtest)
# add_dependencies(histTest gtest)
# add_dependencies(compTest gtest)

target_include_directories(bmpTest PUBLIC ../src)
target_include_directories(histTest PUBLIC ../src)
target_include_directories(compTest PUBLIC ../src)

target_link_libraries(bmpTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(histTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(compTest gtest gtest_main pthread bmplib imglib complib)

gtest_discover_tests(bmpTest)
gtest_discover_tests(histTest)
gtest_discover_tests(compTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} bmpTest histTest --output-on-failure DEPENDS bmpTest histTest compTest)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
