# set
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
# find thirdparty
if(POLICY CMP0146)
  cmake_policy(SET CMP0146 OLD) 
endif()
find_package(CUDA REQUIRED)
list(APPEND CUDA_ALL_LIBS 
  ${CUDA_LIBRARIES} 
  ${CUDA_cublas_LIBRARY} 
  ${CUDA_nppc_LIBRARY} ${CUDA_nppig_LIBRARY} ${CUDA_nppidei_LIBRARY} ${CUDA_nppial_LIBRARY})

# include cuda's header
list(APPEND CUDA_INCLUDE_DIRS ${CUDA_INCLUDE_DIRS})
# message(FATAL_ERROR "CUDA_npp_LIBRARY: ${CUDA_npp_LIBRARY}")

# gather TensorRT lib
set(TensorRT_ROOT /usr/local/TensorRT-8.6.1.6)
# set(TensorRT_ROOT /home/xy/TensorRT-8.5.3.1)

find_library(TRT_NVINFER NAMES nvinfer HINTS ${TensorRT_ROOT} PATH_SUFFIXES lib lib64 lib/x64)
find_library(TRT_NVINFER_PLUGIN NAMES nvinfer_plugin HINTS ${TensorRT_ROOT} PATH_SUFFIXES lib lib64 lib/x64)
find_library(TRT_NVONNX_PARSER NAMES nvonnxparser HINTS ${TensorRT_ROOT} PATH_SUFFIXES lib lib64 lib/x64)
find_library(TRT_NVCAFFE_PARSER NAMES nvcaffe_parser HINTS ${TensorRT_ROOT} PATH_SUFFIXES lib lib64 lib/x64)
find_path(TENSORRT_INCLUDE_DIR NAMES NvInfer.h HINTS ${TensorRT_ROOT} PATH_SUFFIXES include)
list(APPEND CUDA_ALL_LIBS ${TRT_NVINFER} ${TRT_NVINFER_PLUGIN} ${TRT_NVONNX_PARSER} ${TRT_NVCAFFE_PARSER})

# for libtensorrt_sample_common.so
# list(APPEND CUDA_ALL_LIBS ${TensorRT_ROOT}/samples/common/libtensorrt_sample_common.so)
# include tensorrt's headers
list(APPEND CUDA_INCLUDE_DIRS ${TENSORRT_INCLUDE_DIR})

# include tensorrt's sample/common headers
set(SAMPLES_COMMON_DIR ${TensorRT_ROOT}/samples/common)
list(APPEND CUDA_INCLUDE_DIRS ${SAMPLES_COMMON_DIR})
message(STATUS "CUDA_INCLUDE_DIRS: ${CUDA_INCLUDE_DIRS}")
message(STATUS "CUDA_ALL_LIBS: ${CUDA_ALL_LIBS}")
