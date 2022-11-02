
add_library(bmplib bmplib.cpp)
add_library(ppmlib ppmlib.cpp)
add_library(imglib image.cpp)
add_library(complib compressor.cpp)
add_library(colorspaceslib colorspaces.cpp)
add_library(common common.cpp)

add_executable(pg playground.cpp)
add_executable(hist_pg playground_hist.cpp)

target_link_libraries(pg        bmplib imglib)
target_link_libraries(imglib    PRIVATE common bmplib)
target_link_libraries(hist_pg   PRIVATE imglib)
target_link_libraries(complib   PRIVATE imglib)
