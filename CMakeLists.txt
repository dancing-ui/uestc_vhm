cmake_minimum_required(VERSION 3.10)
project(uestc_vhm VERSION 1.0.0 LANGUAGES C CXX CUDA)

include(common_cmake/build_type.cmake)
include(common_cmake/cflags.cmake)
include(common_cmake/format.cmake)
# include(common_cmake/tidy.cmake)
# include(common_cmake/tests.cmake)

add_subdirectory(src)
# add_subdirectory(test)