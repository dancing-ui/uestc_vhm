cmake_minimum_required(VERSION 3.10)

set(SRC_DIR 
    3rd_party
    3rd_party/fast_reid/include
    common 
    config
    database
    database/faiss
    database/rocksdb
    hardware_accelerator/cuda/decode/yolov8
    hardware_accelerator/cuda/kernel_function
    log
    model_handle
    model_handle/model_driver
    model_handle/model_driver/yolov8
    model_handle/model_driver/fast_reid
    model_handle/model_manager
    model_handle/model_service
    network
    network/http_client
    object_track
    object_track/deepsort
    parameter
    person_reid
    stream_media
    stream_media/stream_media_manager
    stream_media/stream_media_driver
    stream_media/stream_media_driver/rtmp
    stream_media/stream_media_driver/rtsp
    utils
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
include(${CMAKE_CURRENT_SOURCE_DIR}/hardware_accelerator/cuda/common.cmake)
include_directories(${CUDA_INCLUDE_DIRS})
# find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

list(APPEND DEPEND_LIB_LIST ${CUDA_ALL_LIBS} ${OpenCV_LIBRARIES})

list(APPEND DEPEND_LIB_LIST ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/fast_reid/lib/libFastRTEngine.so)
# find faiss
include_directories(/usr/local/faiss_gpu/include)
list(APPEND DEPEND_LIB_LIST /usr/local/faiss_gpu/lib/libfaiss.so)
# find rocksdb
include_directories(/usr/local/rocksdb/include)
list(APPEND DEPEND_LIB_LIST /usr/local/rocksdb/lib/librocksdb.so)
# find libhv
include_directories(/usr/local/libhv/include)
list(APPEND DEPEND_LIB_LIST /usr/local/libhv/lib/libhv.so)

message(STATUS "DEPEND_LIB_LIST: ${DEPEND_LIB_LIST}")

add_executable(${PROJECT_NAME} 
    uestc_vhm.cpp
    ${SOURCE_LIST}
    ${TensorRT_ROOT}/samples/common/logger.cpp 
)

target_link_libraries(${PROJECT_NAME} ${DEPEND_LIB_LIST})
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES 72 75 89) # 89 for NVIDIA GeForce RTX 4060
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CUDA>:--default-stream per-thread -lineinfo; --use_fast_math --disable-warnings -Wno-deprecated-gpu-targets>)

else()

message(FATAL_ERROR "Unsupported architecture")

endif()

