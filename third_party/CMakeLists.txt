# Download and unpack libraries at configure time
set(PACKAGES googletest libpng matplotlibcpp)
foreach(package IN LISTS PACKAGES)
  configure_file(CMakeLists_${package}.cmake ${package}-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/${package}-download )
  if(result)
    message(FATAL_ERROR "CMake step for ${package} failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third_party/${package}-download )
  if(result)
    message(FATAL_ERROR "Build step for ${package} failed: ${result}")
  endif()

  # Add package directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_BINARY_DIR}/third_party/${package}-src
                   ${CMAKE_BINARY_DIR}/third_party/${package}-build
                   EXCLUDE_FROM_ALL)

endforeach()

