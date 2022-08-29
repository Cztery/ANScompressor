# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
cmake_minimum_required (VERSION 3.16.3)
find_package(Threads)

add_executable(bmpTest bmpTest.cpp)
add_executable(ppmTest ppmTest.cpp)
# add_executable(colorspacesTest colorspacesTest.cpp)
add_executable(histTest histTest.cpp)
add_executable(compressionTest compressionTest.cpp)

# add_dependencies(bmpTest gtest)
# add_dependencies(histTest gtest)
# add_dependencies(compressionTest gtest)

target_include_directories(bmpTest PUBLIC ../src)
target_include_directories(ppmTest PUBLIC ../src)
# target_include_directories(colorspacesTest PUBLIC ../src)
target_include_directories(histTest PUBLIC ../src)
target_include_directories(compressionTest PUBLIC ../src)

target_link_libraries(bmpTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(ppmTest gtest gtest_main pthread ppmlib imglib)
# target_link_libraries(colorspacesTest gtest gtest_main pthread colorspaceslib)
target_link_libraries(histTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(compressionTest gtest gtest_main pthread ppmlib imglib complib)

gtest_discover_tests(bmpTest)
gtest_discover_tests(ppmTest)
# gtest_discover_tests(colorspacesTest)
gtest_discover_tests(histTest)
gtest_discover_tests(compressionTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                            bmpTest
                            ppmTest
                            #colorspacesTest
                            histTest
                            compressionTest
                            --output-on-failure -j 4
                        DEPENDS
                            bmpTest
                            ppmTest
                            #colorspacesTest
                            histTest
                            compressionTest)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
