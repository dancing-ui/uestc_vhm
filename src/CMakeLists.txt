cmake_minimum_required(VERSION 3.10)

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fsanitize=address -fno-omit-frame-pointer -fsanitize-recover=address -fno-stack-protector -fno-var-tracking")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize-recover=address -fno-stack-protector -fno-var-tracking")
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ggdb -std=c++17 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

set(SRC_DIR 
    common 
    # # hardware_accelerator
    # # hardware_accelerator_driver/cuda/decode/yolov8
    # # hardware_accelerator_driver/cuda/utils
    log
    # # model_handle
    # # model_handle/model_driver/yolov8
    parameter
    # # stream_media
    # # stream_media/stream_media_driver/rtmp
    # # stream_media/stream_media_driver/rtsp
    # utils
    # # video_process
    # # video_process/codec_driver/ffmpeg
    # # video_process/codec_manager
    # # video_process/codec_manager/encoder
)

foreach(DIR ${SRC_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${DIR})
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${DIR} SOURCE_LIST)
endforeach()

message(STATUS "SOURCE_LIST: ${SOURCE_LIST}")

if("${BUILD_PLATFORM}" STREQUAL "x86_64")
add_definitions(-DBUILD_X86_64)
# find cuda
include(${CMAKE_CURRENT_SOURCE_DIR}/hardware_accelerator/hardware_accelerator_driver/cuda/common.cmake)
include_directories(${CUDA_INCLUDE_DIRS})
# find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

list(APPEND DEPEND_LIB_LIST ${CUDA_ALL_LIBS} ${OpenCV_LIBRARIES})

add_executable(${PROJECT_NAME} 
    app/main.cpp
    ${SOURCE_LIST}
    ${TensorRT_ROOT}/samples/common/logger.cpp 
    ${TensorRT_ROOT}/samples/common/sampleOptions.cpp
    ${TensorRT_ROOT}/samples/common/sampleUtils.cpp
)

target_link_libraries(${PROJECT_NAME} ${DEPEND_LIB_LIST})
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES 50 61 72 75 89)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread -lineinfo; --use_fast_math --disable-warnings>)

elseif(not "${BUILD_PLATFORM}" STREQUAL "")

message(FATAL_ERROR "Unsupported architecture")

else()

message(FATAL_ERROR "BUILD_PLATFORM field is empty")

endif()

