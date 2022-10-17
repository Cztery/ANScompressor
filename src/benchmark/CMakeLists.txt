add_compile_options(
       $<$<CONFIG:RELEASE>:-Ofast>
       $<$<CONFIG:DEBUG>:-O0>
       $<$<CONFIG:DEBUG>:-ggdb3>
)

add_library(benchlib benchlib.cpp)

target_compile_options(benchlib PUBLIC
       $<$<CONFIG:RELEASE>:-Ofast>
)

add_executable(bench_all benchmark_all.cpp)
add_executable(profile_stack benchmark_one.cpp)
add_executable(bench_prob_scale bench_prob_scale.cpp)
add_executable(bench_chunk_size bench_chunk_size.cpp)

target_link_libraries(benchlib PRIVATE common complib bmplib ppmlib imglib)
target_link_libraries(bench_all PRIVATE benchlib)
target_link_libraries(profile_stack PRIVATE benchlib)
target_link_libraries(bench_prob_scale PRIVATE benchlib)
target_link_libraries(bench_chunk_size PRIVATE benchlib)

string(TIMESTAMP TS "%Y%m%d_%H%M")

add_custom_command(OUTPUT chunk_size_results_latest.json
       COMMAND bench_chunk_size ARGS chunk_size_results_${TIMESTAMP}
       COMMAND ${CMAKE_COMMAND} -E create_symlink chunk_size_results_${TIMESTAMP}.json chunk_size_results_latest.json
       COMMAND $(CMAKE_SOURCE_DIR)/src/scripts/plot_bench_chunk_size.py ${CMAKE_CURRENT_BINARY_DIR}/chunk_size_results_latest.json
       DEPENDS bench_chunk_size
)

add_custom_target(bench_chunk_size_results
       DEPENDS chunk_size_results_latest.json
)

add_custom_command(OUTPUT prob_scale_results_latest.json
       COMMAND bench_prob_scale ARGS prob_scale_results_${TIMESTAMP}
       COMMAND ${CMAKE_COMMAND} -E create_symlink prob_scale_results_${TIMESTAMP}.json prob_scale_results_latest.json
       COMMAND $(CMAKE_SOURCE_DIR)/src/scripts/plot_bench_prob_scale.py ${CMAKE_CURRENT_BINARY_DIR}/prob_scale_results_latest.json
       DEPENDS bench_prob_scale
)

add_custom_target(bench_prob_scale_results
       DEPENDS chunk_size_results_latest.json
)