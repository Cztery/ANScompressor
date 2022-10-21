enable_language(CUDA)

add_executable(profile_nv benchmark_nv.cu)
add_executable(nv_pg playground_nv.cu)

target_link_libraries(profile_nv benchlib)
target_link_libraries(nv_pg benchlib)