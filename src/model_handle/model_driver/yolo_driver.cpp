#include "yolo_driver.h"

namespace ns_uestc_vhm {

yolo::YOLO::YOLO(ModelCfgItem const &cfg) :
    cfg_(cfg) {
    // input
    m_input_src_device = nullptr;
    m_input_resize_device = nullptr;
    m_input_rgb_device = nullptr;
    m_input_norm_device = nullptr;
    m_input_hwc_device = nullptr;
    checkRuntime(cudaMalloc(&m_input_src_device, cfg.param.batch_size * 3 * cfg.param.src_h * cfg.param.src_w * sizeof(unsigned char)));
    checkRuntime(cudaMalloc(&m_input_resize_device, cfg.param.batch_size * 3 * cfg.param.dst_h * cfg.param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_rgb_device, cfg.param.batch_size * 3 * cfg.param.dst_h * cfg.param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_norm_device, cfg.param.batch_size * 3 * cfg.param.dst_h * cfg.param.dst_w * sizeof(float)));
    checkRuntime(cudaMalloc(&m_input_hwc_device, cfg.param.batch_size * 3 * cfg.param.dst_h * cfg.param.dst_w * sizeof(float)));

    // output
    m_output_src_device = nullptr;
    m_output_objects_device = nullptr;
    m_output_objects_host = nullptr;
    m_output_objects_width = 7;
    m_output_idx_device = nullptr;
    m_output_conf_device = nullptr;
    int output_objects_size = cfg.param.batch_size * (1 + cfg.param.topK * m_output_objects_width); // 1: count
    checkRuntime(cudaMalloc(&m_output_objects_device, output_objects_size * sizeof(float)));
    checkRuntime(cudaMalloc(&m_output_idx_device, cfg.param.batch_size * cfg.param.topK * sizeof(int)));
    checkRuntime(cudaMalloc(&m_output_conf_device, cfg.param.batch_size * cfg.param.topK * sizeof(float)));
    m_output_objects_host = new float[output_objects_size];
    m_objectss.resize(cfg.param.batch_size);
}

yolo::YOLO::~YOLO() {
    // input
    checkRuntime(cudaFree(m_input_src_device));
    checkRuntime(cudaFree(m_input_resize_device));
    checkRuntime(cudaFree(m_input_rgb_device));
    checkRuntime(cudaFree(m_input_norm_device));
    checkRuntime(cudaFree(m_input_hwc_device));
    // output
    checkRuntime(cudaFree(m_output_src_device));
    checkRuntime(cudaFree(m_output_objects_device));
    checkRuntime(cudaFree(m_output_idx_device));
    checkRuntime(cudaFree(m_output_conf_device));
    delete[] m_output_objects_host;
}

int32_t yolo::YOLO::Init() {
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
    if (cfg_.param.dynamic_batch) // for some models only support static mutil-batch. eg: yolox
    {
        this->m_context->setBindingDimensions(0, nvinfer1::Dims4(cfg_.param.batch_size, 3, cfg_.param.dst_h, cfg_.param.dst_w));
    }
    m_output_dims = this->m_context->getBindingDimensions(1);
    m_total_objects = m_output_dims.d[1];
    assert(cfg_.param.batch_size <= m_output_dims.d[0]);
    m_output_area = 1;
    for (int i = 1; i < m_output_dims.nbDims; i++) {
        if (m_output_dims.d[i] != 0) {
            m_output_area *= m_output_dims.d[i];
        }
    }
    checkRuntime(cudaMalloc(&m_output_src_device, cfg_.param.batch_size * m_output_area * sizeof(float)));
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

int32_t yolo::YOLO::BatchInference(std::vector<cv::Mat> &imgs_batch) {
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

void yolo::YOLO::check() {
    int idx;
    nvinfer1::Dims dims;

    sample::gLogInfo << "the engine's info:" << std::endl;
    for (auto layer_name : cfg_.param.input_output_names) {
        idx = this->m_engine->getBindingIndex(layer_name.c_str());
        dims = this->m_engine->getBindingDimensions(idx);
        sample::gLogInfo << "idx = " << idx << ", " << layer_name << ": ";
        for (int i = 0; i < dims.nbDims; i++) {
            sample::gLogInfo << dims.d[i] << ", ";
        }
        sample::gLogInfo << std::endl;
    }
    sample::gLogInfo << "the context's info:" << std::endl;
    for (auto layer_name : cfg_.param.input_output_names) {
        idx = this->m_engine->getBindingIndex(layer_name.c_str());
        dims = this->m_context->getBindingDimensions(idx);
        sample::gLogInfo << "idx = " << idx << ", " << layer_name << ": ";
        for (int i = 0; i < dims.nbDims; i++) {
            sample::gLogInfo << dims.d[i] << ", ";
        }
        sample::gLogInfo << std::endl;
    }
}
void yolo::YOLO::copy(const std::vector<cv::Mat> &imgsBatch) {
#if 0 
    cv::Mat img_fp32 = cv::Mat::zeros(imgsBatch[0].size(), CV_32FC3); // todo 
    cudaHostRegister(img_fp32.data, img_fp32.elemSize() * img_fp32.total(), cudaHostRegisterPortable);
    float* pi = m_input_src_device;
    for (size_t i = 0; i < imgsBatch.size(); i++)
    {
        imgsBatch[i].convertTo(img_fp32, CV_32FC3);
        checkRuntime(cudaMemcpy(pi, img_fp32.data, sizeof(float) * 3 * cfg_.param.src_h * cfg_.param.src_w, cudaMemcpyHostToDevice));
        pi += 3 * cfg_.param.src_h * cfg_.param.src_w;
    }
    cudaHostUnregister(img_fp32.data);
#endif

#if 0 // for Nvidia TX2
    cv::Mat img_fp32 = cv::Mat::zeros(imgsBatch[0].size(), CV_32FC3); // todo 
    float* pi = m_input_src_device;
    for (size_t i = 0; i < imgsBatch.size(); i++)
    {
        std::vector<float> img_vec = std::vector<float>(imgsBatch[i].reshape(1, 1));
        imgsBatch[i].convertTo(img_fp32, CV_32FC3);
        checkRuntime(cudaMemcpy(pi, img_fp32.data, sizeof(float) * 3 * cfg_.param.src_h * cfg_.param.src_w, cudaMemcpyHostToDevice));
        pi += 3 * cfg_.param.src_h * cfg_.param.src_w;
    }
#endif

    // update 20230302, faster.
    // 1. move uint8_to_float in cuda kernel function. For 8*3*1920*1080, cost time 15ms -> 3.9ms
    // 2. Todo
    unsigned char *pi = m_input_src_device;
    for (size_t i = 0; i < imgsBatch.size(); i++) {
        checkRuntime(cudaMemcpy(pi, imgsBatch[i].data, sizeof(unsigned char) * 3 * cfg_.param.src_h * cfg_.param.src_w, cudaMemcpyHostToDevice));
        pi += 3 * cfg_.param.src_h * cfg_.param.src_w;
    }

#if 0 // cuda stream
    cudaStream_t streams[32];
    for (int i = 0; i < imgsBatch.size(); i++) 
    {
        checkRuntime(cudaStreamCreate(&streams[i]));
    }
    unsigned char* pi = m_input_src_device;
    for (size_t i = 0; i < imgsBatch.size(); i++)
    {
        checkRuntime(cudaMemcpyAsync(pi, imgsBatch[i].data, sizeof(unsigned char) * 3 * cfg_.param.src_h * cfg_.param.src_w, cudaMemcpyHostToDevice, streams[i]));
        pi += 3 * cfg_.param.src_h * cfg_.param.src_w;
    }
    checkRuntime(cudaDeviceSynchronize());
#endif
}

void yolo::YOLO::preprocess(const std::vector<cv::Mat> &imgsBatch) {
    resizeDevice(cfg_.param.batch_size, m_input_src_device, cfg_.param.src_w, cfg_.param.src_h,
                 m_input_resize_device, cfg_.param.dst_w, cfg_.param.dst_h, 114, m_dst2src);
    bgr2rgbDevice(cfg_.param.batch_size, m_input_resize_device, cfg_.param.dst_w, cfg_.param.dst_h,
                  m_input_rgb_device, cfg_.param.dst_w, cfg_.param.dst_h);
    normDevice(cfg_.param.batch_size, m_input_rgb_device, cfg_.param.dst_w, cfg_.param.dst_h,
               m_input_norm_device, cfg_.param.dst_w, cfg_.param.dst_h, cfg_.param);
    hwc2chwDevice(cfg_.param.batch_size, m_input_norm_device, cfg_.param.dst_w, cfg_.param.dst_h,
                  m_input_hwc_device, cfg_.param.dst_w, cfg_.param.dst_h);
}

bool yolo::YOLO::infer() {
    float *bindings[] = {m_input_hwc_device, m_output_src_device};
    bool context = m_context->executeV2((void **)bindings);
    return context;
}

void yolo::YOLO::postprocess(const std::vector<cv::Mat> &imgsBatch) {
    decodeDevice(cfg_.param, m_output_src_device, 5 + cfg_.param.num_class, m_total_objects, m_output_area,
                 m_output_objects_device, m_output_objects_width, cfg_.param.topK);

    // nmsv1(nms faster)
    nmsDeviceV1(cfg_.param, m_output_objects_device, m_output_objects_width, cfg_.param.topK, cfg_.param.topK * m_output_objects_width + 1);

    // nmsv2(nms sort)
    // nmsDeviceV2(cfg_.param, m_output_objects_device, m_output_objects_width, cfg_.param.topK, cfg_.param.topK * m_output_objects_width + 1, m_output_idx_device, m_output_conf_device);

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

std::vector<std::vector<utils::Box>> yolo::YOLO::getObjectss() const {
    return this->m_objectss;
}

void yolo::YOLO::reset() {
    checkRuntime(cudaMemset(m_output_objects_device, 0, sizeof(float) * cfg_.param.batch_size * (1 + 7 * cfg_.param.topK)));
    for (size_t bi = 0; bi < cfg_.param.batch_size; bi++) {
        m_objectss[bi].clear();
    }
}

bool yolo::YOLO::CheckIsPerson(int32_t label) const {
    if (cfg_.param.num_class == 91 || cfg_.param.num_class == 80) {
        return label == 0;
    } else if (cfg_.param.num_class == 20) {
        return label == 14;
    }
    return false;
}

} // ns_uestc_vhm
