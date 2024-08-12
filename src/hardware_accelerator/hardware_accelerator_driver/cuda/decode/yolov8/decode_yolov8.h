#ifndef _UESTC_VHM_DECODE_YOLOV8_H_
#define _UESTC_VHM_DECODE_YOLOV8_H_
#include "utils.h"
#include "kernel_function.h"

namespace yolov8 {
void decodeDevice(utils::InitParameter param, float *src, int srcWidth, int srcHeight, int srcLength, float *dst, int dstWidth, int dstHeight);
void transposeDevice(utils::InitParameter param, float *src, int srcWidth, int srcHeight, int srcArea, float *dst, int dstWidth, int dstHeight);
}
#endif // _UESTC_VHM_DECODE_YOLOV8_H_