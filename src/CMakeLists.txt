set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_library(bmplib bmplib.cpp)
add_library(ppmlib ppmlib.cpp)
add_library(imglib image.cpp)
add_library(complib compressor.cpp)
add_library(colorspaceslib colorspaces.cpp)
add_library(common common.cpp)
add_library(benchlib benchlib.cpp)

add_executable(pg playground.cpp)
add_executable(hist_pg playground_hist.cpp)

add_executable(bench_all benchmark_all.cpp)
add_executable(bench_prob_scale bench_prob_scale.cpp)
target_compile_options(benchlib PRIVATE
       -Wall -Werror -Wno-error=maybe-uninitialized
       $<$<CONFIG:RELEASE>:-Ofast>
)

target_link_libraries(pg        bmplib imglib)
target_link_libraries(imglib    PRIVATE common bmplib)
target_link_libraries(hist_pg   PRIVATE imglib python3.10)
target_link_libraries(complib   PRIVATE imglib)
target_link_libraries(benchlib  PRIVATE common complib bmplib ppmlib imglib)
target_link_libraries(bench_all benchlib)
target_link_libraries(bench_prob_scale benchlib python3.10)
target_include_directories(hist_pg PRIVATE /usr/include/python3.10 ${CMAKE_BINARY_DIR}/third_party/matplotlibcpp-src)
target_include_directories(bench_prob_scale PRIVATE /usr/include/python3.10 ${CMAKE_BINARY_DIR}/third_party/matplotlibcpp-src)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND $<CONFIG:RELEASE>)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
    message("setting IPO")
endif()
