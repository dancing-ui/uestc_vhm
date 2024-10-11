#ifndef _UESTC_VHM_YOLOV8_H_
#define _UESTC_VHM_YOLOV8_H_
#include "yolo_driver.h"

namespace ns_uestc_vhm {

class YOLOV8 : public yolo::YOLO {
public:
    YOLOV8(ModelCfgItem const &cfg);
    ~YOLOV8();

    YOLOV8(YOLOV8 const &) = delete;
    YOLOV8 &operator=(YOLOV8 const &) = delete;
    YOLOV8(YOLOV8 &&) = delete;
    YOLOV8 &operator=(YOLOV8 &&) = delete;

    int32_t Init() override;
    int32_t BatchInference(std::vector<cv::Mat> &imgs_batch) override;
    void preprocess(const std::vector<cv::Mat> &imgsBatch) override;
    void postprocess(const std::vector<cv::Mat> &imgsBatch) override;

private:
    float *m_output_src_transpose_device;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_YOLOV8_H_