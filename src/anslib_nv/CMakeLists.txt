enable_language(CUDA)

set(CMAKE_CUDA_FLAGS_DEBUG "-O0 -g -G -gencode arch=compute_70,code=sm_70")
set(CMAKE_CUDA_FLAGS_RELEASE "-O3")
#add_compile_options(-arch compute_70 -O3)
add_executable(profile_nv benchmark_nv.cu)
add_executable(nv_pg playground_nv.cu)

message("CMAKE_CUDA_FLAGS is ${CMAKE_CUDA_FLAGS_RELEASE}")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS_RELEASE}")

#string(REPLACE "-Ofast" "-O3" CMAKE_CUDA_FLAGS CMAKE_CUDA_FLAGS)
#set(CMAKE_CUDA_ARCHITECTURES 70 53)


target_link_libraries(profile_nv benchlib)
target_link_libraries(nv_pg benchlib imglib ppmlib bmplib common)