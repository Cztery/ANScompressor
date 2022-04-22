add_library(bmplib bmplib.cpp)
add_library(anslib ansComp.cpp)

add_executable(pg playground.cpp)
add_executable(hist_pg playground_hist.cpp)

target_link_libraries(pg bmplib anslib)
target_link_libraries(hist_pg bmplib anslib)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)