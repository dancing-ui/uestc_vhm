#ifndef _UESTC_VHM_KERNEL_FUNCTION_H_
#define _UESTC_VHM_KERNEL_FUNCTION_H_
#include "utils.h"

namespace ns_uestc_vhm {

#define checkRuntime(op) __check_cuda_runtime((op), #op, __FILE__, __LINE__)

bool __check_cuda_runtime(cudaError_t code, const char *op, const char *file, int line);

#define BLOCK_SIZE 8

// note: resize rgb with padding
void resizeDevice(const int &batch_size, float *src, int src_width, int src_height,
                  float *dst, int dstWidth, int dstHeight,
                  float paddingValue, utils::AffineMat matrix);

// overload:resize rgb with padding, but src's type is uin8
void resizeDevice(const int &batch_size, unsigned char *src, int src_width, int src_height,
                  float *dst, int dstWidth, int dstHeight,
                  float paddingValue, utils::AffineMat matrix);

// overload: resize rgb/gray without padding
void resizeDevice(const int &batchSize, float *src, int srcWidth, int srcHeight,
                  float *dst, int dstWidth, int dstHeight,
                  utils::ColorMode mode, utils::AffineMat matrix);

void bgr2rgbDevice(const int &batch_size, float *src, int srcWidth, int srcHeight,
                   float *dst, int dstWidth, int dstHeight);

void normDevice(const int &batch_size, float *src, int srcWidth, int srcHeight,
                float *dst, int dstWidth, int dstHeight,
                InitParameter norm_param);

void hwc2chwDevice(const int &batch_size, float *src, int srcWidth, int srcHeight,
                   float *dst, int dstWidth, int dstHeight);

void decodeDevice(InitParameter param, float *src, int srcWidth, int srcHeight, int srcLength, float *dst, int dstWidth, int dstHeight);

// nms fast
void nmsDeviceV1(InitParameter param, float *src, int srcWidth, int srcHeight, int srcArea);

// nms sort
void nmsDeviceV2(InitParameter param, float *src, int srcWidth, int srcHeight, int srcArea,
                 int *idx, float *conf);

void copyWithPaddingDevice(const int &batchSize, float *src, int srcWidth, int srcHeight,
                           float *dst, int dstWidth, int dstHeight, float paddingValue, int padTop, int padLeft);

} // ns_uestc_vhm

#endif // _UESTC_VHM_KERNEL_FUNCTION_H_