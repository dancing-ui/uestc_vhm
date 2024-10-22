cmake_minimum_required(VERSION 3.10)
project(uestc_vhm VERSION 1.0.0 LANGUAGES C CXX CUDA)

include(cmake/build_type.cmake)
include(cmake/cflags.cmake)
include(cmake/format.cmake)
include(cmake/cppcheck.cmake)
# include(cmake/tidy.cmake)
# include(cmake/tests.cmake)

add_subdirectory(src)
# add_subdirectory(test)