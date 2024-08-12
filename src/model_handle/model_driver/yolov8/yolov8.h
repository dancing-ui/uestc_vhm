#ifndef _UESTC_VHM_YOLOV8_H_
#define _UESTC_VHM_YOLOV8_H_
#include "yolo.h"
#include "utils.h"
class YOLOV8 : public yolo::YOLO {
public:
    YOLOV8(const utils::InitParameter &param);
    ~YOLOV8();
    virtual bool init(const std::vector<unsigned char> &trtFile);
    virtual void preprocess(const std::vector<cv::Mat> &imgsBatch);
    virtual void postprocess(const std::vector<cv::Mat> &imgsBatch);

private:
    float *m_output_src_transpose_device;
};

#endif // _UESTC_VHM_YOLOV8_H_