add_library(bmplib bmplib.cpp)
add_library(anslib ansComp.cpp)

add_executable(ansstuff playground.cpp)
add_executable(hist histogram.cpp)

target_link_libraries(ansstuff bmplib anslib)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")