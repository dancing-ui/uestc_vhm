#ifndef _UESTC_VHM_DECODE_YOLOV8_H_
#define _UESTC_VHM_DECODE_YOLOV8_H_
#include "utils.h"
#include "kernel_function.h"
#include "parameter.h"

namespace ns_uestc_vhm {

namespace yolov8 {
void decodeDevice(InitParameter param, float *src, int srcWidth, int srcHeight, int srcLength, float *dst, int dstWidth, int dstHeight);
void transposeDevice(InitParameter param, float *src, int srcWidth, int srcHeight, int srcArea, float *dst, int dstWidth, int dstHeight);

} // yolov8

} // ns_uestc_vhm
#endif // _UESTC_VHM_DECODE_YOLOV8_H_