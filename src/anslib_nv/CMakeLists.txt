enable_language(CUDA)

set(CMAKE_CUDA_FLAGS_DEBUG "-O0 -g -G -gencode arch=compute_70,code=sm_70")
set(CMAKE_CUDA_FLAGS_RELEASE "-O3")

add_library(imglib_nv image.cu)

add_executable(profile_nv benchmark_nv.cu) 
add_executable(nv_pg playground_nv.cu)

target_link_libraries(imglib_nv imglib)
target_link_libraries(nv_pg imglib_nv)

message("CMAKE_CUDA_FLAGS is ${CMAKE_CUDA_FLAGS_RELEASE}")
message("MAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS_RELEASE}")

target_link_libraries(profile_nv benchlib)
target_link_libraries(nv_pg benchlib imglib ppmlib bmplib common)