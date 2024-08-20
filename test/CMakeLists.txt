cmake_minimum_required(VERSION 3.10)
project(test)

set(SRC_DIR 
    algorithm
    cache
    comparator
    compression
    data_structure
    db_format
    env
    filter_policy
    iterator
    log
    log_reader
    log_writer
    memory
    options
    snapshot
    table
    util
    version
    write_batch
)

foreach(DIR ${SRC_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/${DIR})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../src/${DIR} SOURCE_LIST)
endforeach()

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(fdatasync "unistd.h" CMAKE_HAVE_FDATASYNC)
check_cxx_symbol_exists(F_FULLFSYNC "fcntl.h" CMAKE_HAVE_FULLFSYNC)
check_cxx_symbol_exists(O_CLOEXEC "fcntl.h" CMAKE_HAVE_O_CLOEXEC)
if (CMAKE_HAVE_FDATASYNC)
    message("[INFO] [TEST] Found fdatasync")
    add_definitions(-DHAVE_FDATASYNC)
endif()
if (CMAKE_HAVE_FULLFSYNC)
    message("[INFO] [TEST] Found F_FULLFSYNC")
    add_definitions(-DHAVE_FULLFSYNC)
endif()
if (CMAKE_HAVE_O_CLOEXEC)
    message("[INFO] [TEST] Found O_CLOEXEC")
    add_definitions(-DHAVE_O_CLOEXEC)
endif()

set(DEPEND_LIB "")

find_library(CRC32C_LIBRARY NAMES crc32c PATHS "/usr/local/lib")
if (CRC32C_LIBRARY)
    message("[INFO] [TEST] Found crc32c")
    add_definitions(-DHAVE_CRC32C)
    list(APPEND DEPEND_LIB crc32c)
endif()

find_library(SNAPPY_LIBRARY NAMES snappy PATHS "/usr/local/lib")
if(SNAPPY_LIBRARY)
    message("[INFO] [TEST] Found snappy")
    add_definitions(-DHAVE_SNAPPY)
    list(APPEND DEPEND_LIB snappy)
endif()

find_library(ZSTD_LIBRARY NAMES zstd PATHS "/usr/local/lib")
if(ZSTD_LIBRARY)
    message("[INFO] [TEST] Found zstd")
    add_definitions(-DHAVE_ZSTD)
    list(APPEND DEPEND_LIB zstd)
endif()

if("${TEST_FILE_NAME}" STREQUAL "test_")
    
else()
    list(APPEND DEPEND_LIB pthread gtest gmock)
    add_executable(${TEST_FILE_NAME} 
        ${TEST_FILE_NAME}.cpp
        ${SOURCE_LIST}
    )
    target_link_libraries(${TEST_FILE_NAME} ${DEPEND_LIB})
endif()