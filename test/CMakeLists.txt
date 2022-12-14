# Download and unpack googletest at configure time
#configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
cmake_minimum_required (VERSION 3.16.3)
find_package(Threads)

add_executable(anslibNvTest anslibNvTest.cpp)
add_executable(bmpTest bmpTest.cpp)
# add_executable(colorspacesTest colorspacesTest.cpp)
add_executable(compressionTest compressionTest.cpp)
add_executable(histTest histTest.cpp)
add_executable(imgTest imageTest.cpp)
add_executable(ppmTest ppmTest.cpp)

target_include_directories(anslibNvTest PUBLIC ../src/include)
target_include_directories(bmpTest PUBLIC ../src/include)
# target_include_directories(colorspacesTest PUBLIC ../src/include)
target_include_directories(compressionTest PUBLIC ../src/include)
target_include_directories(histTest PUBLIC ../src/include)
target_include_directories(imgTest PUBLIC ../src/include)
target_include_directories(ppmTest PUBLIC ../src/include)

target_link_libraries(anslibNvTest gtest gtest_main pthread imglib imglib_nv)
target_link_libraries(bmpTest gtest gtest_main pthread bmplib imglib)
# target_link_libraries(colorspacesTest gtest gtest_main pthread colorspaceslib)
target_link_libraries(compressionTest gtest gtest_main pthread ppmlib imglib complib)
target_link_libraries(histTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(imgTest gtest gtest_main pthread bmplib imglib)
target_link_libraries(ppmTest gtest gtest_main pthread ppmlib imglib)

gtest_discover_tests(anslibNvTest)
gtest_discover_tests(bmpTest)
# gtest_discover_tests(colorspacesTest)
gtest_discover_tests(compressionTest)
gtest_discover_tests(histTest)
gtest_discover_tests(imgTest)
gtest_discover_tests(ppmTest)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                            anslibNvTest
                            bmpTest
                            #colorspacesTest
                            compressionTest
                            histTest
                            imgTest
                            ppmTest
                            --output-on-failure -j 4
                        DEPENDS
                            anslibNvTest
                            bmpTest
                            #colorspacesTest
                            compressionTest
                            histTest
                            imgTest
                            ppmTest)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
