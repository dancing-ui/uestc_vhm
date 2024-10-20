#include "yolov8.h"
#include "decode_yolov8.h"

namespace ns_uestc_vhm {

YOLOV8::YOLOV8(ModelCfgItem const &cfg) :
    yolo::YOLO(cfg) {
}

YOLOV8::~YOLOV8() {
    checkRuntime(cudaFree(m_output_src_transpose_device));
}

int32_t YOLOV8::Init() {
    // load model
    std::vector<unsigned char> trt_file = utils::loadModel(cfg_.param.model_path);
    if (trt_file.empty()) {
        return -1;
    }
    this->m_runtime = std::unique_ptr<nvinfer1::IRuntime>(nvinfer1::createInferRuntime(sample::gLogger.getTRTLogger()));
    if (this->m_runtime == nullptr) {
        return -2;
    }
    this->m_engine = std::unique_ptr<nvinfer1::ICudaEngine>(this->m_runtime->deserializeCudaEngine(trt_file.data(), trt_file.size()));

    if (this->m_engine == nullptr) {
        return -3;
    }
    this->m_context = std::unique_ptr<nvinfer1::IExecutionContext>(this->m_engine->createExecutionContext());
    if (this->m_context == nullptr) {
        return -4;
    }
    if (cfg_.param.dynamic_batch) {
        this->m_context->setBindingDimensions(0, nvinfer1::Dims4(cfg_.param.batch_size, 3, cfg_.param.dst_h, cfg_.param.dst_w));
    }
    m_output_dims = this->m_context->getBindingDimensions(1);
    m_total_objects = m_output_dims.d[2];
    assert(cfg_.param.batch_size <= m_output_dims.d[0]);
    m_output_area = 1;
    for (int i = 1; i < m_output_dims.nbDims; i++) {
        if (m_output_dims.d[i] != 0) {
            m_output_area *= m_output_dims.d[i];
        }
    }
    checkRuntime(cudaMalloc(&m_output_src_device, cfg_.param.batch_size * m_output_area * sizeof(float)));
    checkRuntime(cudaMalloc(&m_output_src_transpose_device, cfg_.param.batch_size * m_output_area * sizeof(float)));
    float a = float(cfg_.param.dst_h) / cfg_.param.src_h;
    float b = float(cfg_.param.dst_w) / cfg_.param.src_w;
    float scale = a < b ? a : b;
    cv::Mat src2dst = (cv::Mat_<float>(2, 3) << scale, 0.f, (-scale * cfg_.param.src_w + cfg_.param.dst_w + scale - 1) * 0.5,
                       0.f, scale, (-scale * cfg_.param.src_h + cfg_.param.dst_h + scale - 1) * 0.5);
    cv::Mat dst2src = cv::Mat::zeros(2, 3, CV_32FC1);
    cv::invertAffineTransform(src2dst, dst2src);

    m_dst2src.v0 = dst2src.ptr<float>(0)[0];
    m_dst2src.v1 = dst2src.ptr<float>(0)[1];
    m_dst2src.v2 = dst2src.ptr<float>(0)[2];
    m_dst2src.v3 = dst2src.ptr<float>(1)[0];
    m_dst2src.v4 = dst2src.ptr<float>(1)[1];
    m_dst2src.v5 = dst2src.ptr<float>(1)[2];

    return 0;
}

int32_t YOLOV8::BatchInference(std::vector<cv::Mat> &imgs_batch) {
    int32_t ret{0};
    utils::DeviceTimer d_t0;
    copy(imgs_batch);
    double t0 = d_t0.getUsedTime();
    utils::DeviceTimer d_t1;
    preprocess(imgs_batch);
    double t1 = d_t1.getUsedTime();
    utils::DeviceTimer d_t2;
    infer();
    double t2 = d_t2.getUsedTime();
    utils::DeviceTimer d_t3;
    postprocess(imgs_batch);
    double t3 = d_t3.getUsedTime();
    // sample::gLogInfo << "pre_process time = " << t1 / cfg_.param.batch_size << "ms; "
    //                  << "infer time = " << t2 / cfg_.param.batch_size << "ms; "
    //                  << "post_process time = " << t3 / cfg_.param.batch_size << "ms; "
    //                  << std::endl;
    return 0;
}

void YOLOV8::preprocess(const std::vector<cv::Mat> &imgsBatch) {
    resizeDevice(cfg_.param.batch_size, m_input_src_device, cfg_.param.src_w, cfg_.param.src_h,
                 m_input_resize_device, cfg_.param.dst_w, cfg_.param.dst_h, 114, m_dst2src);
    bgr2rgbDevice(cfg_.param.batch_size, m_input_resize_device, cfg_.param.dst_w, cfg_.param.dst_h,
                  m_input_rgb_device, cfg_.param.dst_w, cfg_.param.dst_h);
    normDevice(cfg_.param.batch_size, m_input_rgb_device, cfg_.param.dst_w, cfg_.param.dst_h,
               m_input_norm_device, cfg_.param.dst_w, cfg_.param.dst_h, cfg_.param);
    hwc2chwDevice(cfg_.param.batch_size, m_input_norm_device, cfg_.param.dst_w, cfg_.param.dst_h,
                  m_input_hwc_device, cfg_.param.dst_w, cfg_.param.dst_h);
}

void YOLOV8::postprocess(const std::vector<cv::Mat> &imgsBatch) {
    yolov8::transposeDevice(cfg_.param, m_output_src_device, m_total_objects, 4 + cfg_.param.num_class, m_total_objects * (4 + cfg_.param.num_class),
                            m_output_src_transpose_device, 4 + cfg_.param.num_class, m_total_objects);
    yolov8::decodeDevice(cfg_.param, m_output_src_transpose_device, 4 + cfg_.param.num_class, m_total_objects, m_output_area,
                         m_output_objects_device, m_output_objects_width, cfg_.param.topK);
    // nms
    // nmsDeviceV1(cfg_.param, m_output_objects_device, m_output_objects_width, cfg_.param.topK, cfg_.param.topK * m_output_objects_width + 1);
    nmsDeviceV2(cfg_.param, m_output_objects_device, m_output_objects_width, cfg_.param.topK, cfg_.param.topK * m_output_objects_width + 1, m_output_idx_device, m_output_conf_device);
    checkRuntime(cudaMemcpy(m_output_objects_host, m_output_objects_device, cfg_.param.batch_size * sizeof(float) * (1 + 7 * cfg_.param.topK), cudaMemcpyDeviceToHost));
    for (size_t bi = 0; bi < imgsBatch.size(); bi++) {
        int num_boxes = std::min((int)(m_output_objects_host + bi * (cfg_.param.topK * m_output_objects_width + 1))[0], cfg_.param.topK);
        for (int i = 0; i < num_boxes; i++) {
            float *ptr = m_output_objects_host + bi * (cfg_.param.topK * m_output_objects_width + 1) + m_output_objects_width * i + 1;
            int keep_flag = ptr[6];
            if (keep_flag) {
                if (!CheckIsPerson(ptr[5])) { // not person
                    continue;
                }
                float x_lt = m_dst2src.v0 * ptr[0] + m_dst2src.v1 * ptr[1] + m_dst2src.v2;
                float y_lt = m_dst2src.v3 * ptr[0] + m_dst2src.v4 * ptr[1] + m_dst2src.v5;
                float x_rb = m_dst2src.v0 * ptr[2] + m_dst2src.v1 * ptr[3] + m_dst2src.v2;
                float y_rb = m_dst2src.v3 * ptr[2] + m_dst2src.v4 * ptr[3] + m_dst2src.v5;
                m_objectss[bi].emplace_back(x_lt, y_lt, x_rb, y_rb, ptr[4], (int)ptr[5]);
            }
        }
    }
}

} // ns_uestc_vhm