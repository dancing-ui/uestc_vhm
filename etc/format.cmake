file (GLOB_RECURSE ALL_SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/test/*.h ${CMAKE_SOURCE_DIR}/test/*.cpp)
message(STATUS "ALL_SRC_FILES=${ALL_SRC_FILES}")

add_custom_target (format "clang-format" -i ${ALL_SRC_FILES} COMMENT "Formatting source code...")