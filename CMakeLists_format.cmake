set(SRC_FILE_EXTENSIONS h hpp cuh c cpp cxx cu)
set(SRC_DIRS src test)
find_program(CLANG_FORMAT_PROGRAM clang-format)

foreach(dir ${SRC_DIRS})
    foreach(ext ${SRC_FILE_EXTENSIONS})
        file(GLOB_RECURSE FOUND_FILES ${CMAKE_SOURCE_DIR}/${dir}/*.${ext})
        list(APPEND formatables ${FOUND_FILES})
    endforeach()
endforeach()

add_custom_target(format
COMMAND       
    ${CLANG_FORMAT_PROGRAM} -i
    ${formatables}
WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
)

