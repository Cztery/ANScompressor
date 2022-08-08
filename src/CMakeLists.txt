set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

add_library(bmplib bmplib.cpp)
add_library(imglib image.cpp)
add_library(complib compressor.cpp)

add_executable(pg playground.cpp)
add_executable(hist_pg playground_hist.cpp)

add_executable(bench benchmark.cpp)
target_compile_options(bench PRIVATE
       -Wall -Werror -Wno-error=maybe-uninitialized
       $<$<CONFIG:RELEASE>:-Ofast>
)

target_link_libraries(pg PRIVATE bmplib imglib)
target_link_libraries(hist_pg PRIVATE bmplib imglib python3.10)
target_link_libraries(bench complib bmplib imglib)
target_link_libraries(complib imglib)
target_include_directories(hist_pg PRIVATE /usr/include/python3.10 ${CMAKE_BINARY_DIR}/third_party/matplotlibcpp-src)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND $<CONFIG:RELEASE>)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
    message("setting IPO")
endif()