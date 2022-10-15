set(CMAKE_CXX_STANDARD 17)
set(CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

include_directories(include)
add_subdirectory(anslib)
add_subdirectory(anslib_nv)
add_subdirectory(benchmark)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported AND $<CONFIG:RELEASE>)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION True)
    message("setting IPO")
endif()
