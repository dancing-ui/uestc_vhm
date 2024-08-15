#ifndef _UESTC_VHM_YOLO_H_
#define _UESTC_VHM_YOLO_H_
#include "utils.h"
#include "kernel_function.h"
#include "model_handle_common.h"

namespace ns_uestc_vhm {

namespace yolo {
class YOLO {
public:
    YOLO(ModelCfgItem const &cfg);
    ~YOLO();

    YOLO(YOLO const &) = default;
    YOLO &operator=(YOLO const &) = default;

public:
    virtual int32_t Init();
    virtual int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batch_nums, ModelHandleCb const& handle_cb);
    virtual void check();
    virtual void copy(const std::vector<cv::Mat> &imgsBatch);
    virtual void preprocess(const std::vector<cv::Mat> &imgsBatch);
    virtual bool infer();
    virtual void postprocess(const std::vector<cv::Mat> &imgsBatch);
    virtual void reset();

public:
    std::vector<std::vector<utils::Box>> getObjectss() const;

protected:
    std::shared_ptr<nvinfer1::ICudaEngine> m_engine;
    std::unique_ptr<nvinfer1::IExecutionContext> m_context;

protected:
    ModelCfgItem cfg_;
    nvinfer1::Dims m_output_dims;
    int m_output_area;
    int m_total_objects;
    std::vector<std::vector<utils::Box>> m_objectss;
    utils::AffineMat m_dst2src;

    // input
    unsigned char *m_input_src_device;
    float *m_input_resize_device;
    float *m_input_rgb_device;
    float *m_input_norm_device;
    float *m_input_hwc_device;
    // output
    float *m_output_src_device;
    float *m_output_objects_device;
    float *m_output_objects_host;
    int m_output_objects_width;
    int *m_output_idx_device;
    float *m_output_conf_device;
};
} // yolo

} // ns_uestc_vhm

#endif // _UESTC_VHM_YOLO_H_