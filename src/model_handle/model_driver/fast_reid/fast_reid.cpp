#include "fast_reid.h"
#include "fastrt/baseline.h"
#include "fastrt/utils.h"
#include "log.h"
#include <cstdint>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace ns_uestc_vhm {
FASTREID::FASTREID(ModelCfgItem const &cfg) :
    reid::REID(cfg) {
}
FASTREID::~FASTREID() {
}
int32_t FASTREID::Init() {
    baseline_ = std::make_unique<fastrt::Baseline>(trt::ModelConfig{
        "",
        cfg_.feature_extract_param.batch_size,
        cfg_.feature_extract_param.input_height,
        cfg_.feature_extract_param.input_width,
        cfg_.feature_extract_param.output_size,
        cfg_.feature_extract_param.device_id});
    if (baseline_.get() == nullptr) {
        PRINT_ERROR("create baseline_ failed\n");
        return -1;
    }
    if (!baseline_->deserializeEngine(cfg_.feature_extract_param.engine_path)) {
        PRINT_ERROR("deserialize fast_reid engine file(%s) failed\n", cfg_.feature_extract_param.engine_path.c_str());
        return -2;
    }
    return 0;
}

int32_t FASTREID::BatchInference(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes) {
    if (baseline_.get() == nullptr) {
        PRINT_ERROR("baseline_ is nullptr\n");
        return -1;
    }
    /* Split Image */
    assert(imgs_batch.size() == detect_boxes.size());
    std::vector<std::vector<cv::Mat>> sub_imgs_batch;
    for (size_t i = 0; i < imgs_batch.size(); i++) {
        std::vector<cv::Mat> tmp_sub_imgs_batch;
        for (size_t j = 0; j < detect_boxes[i].size(); j++) {
            int32_t img_width = imgs_batch[i].cols;
            int32_t img_height = imgs_batch[i].rows;
            float left = std::max(detect_boxes[i][j].left, 0.0f);
            float top = std::max(detect_boxes[i][j].top, 0.0f);
            float width = std::min(detect_boxes[i][j].right - detect_boxes[i][j].left, static_cast<float>(img_width - left));
            float height = std::min(detect_boxes[i][j].bottom - detect_boxes[i][j].top, static_cast<float>(img_height - top));
            cv::Rect2f crop_rect{left, top, width, height};
            tmp_sub_imgs_batch.emplace_back(imgs_batch[i](crop_rect).clone());
        }
        sub_imgs_batch.emplace_back(tmp_sub_imgs_batch);
    }

    /* feature extraction */
    feats_lists_.clear();
    for (size_t i = 0; i < sub_imgs_batch.size(); i++) {
        std::vector<cv::Mat> input;
        std::vector<cv::Mat> feats;
        for (size_t batch_start = 0; batch_start < sub_imgs_batch[i].size(); batch_start += cfg_.feature_extract_param.batch_size) {
            input.clear();
            /* collect batch */
            for (int32_t img_idx = 0; img_idx < cfg_.feature_extract_param.batch_size; ++img_idx) {
                if ((batch_start + img_idx) >= sub_imgs_batch[i].size()) continue;
                cv::Mat resizeImg(cfg_.feature_extract_param.input_height, cfg_.feature_extract_param.input_width, CV_8UC3);
                cv::resize(sub_imgs_batch[i][batch_start + img_idx], resizeImg, resizeImg.size(), 0, 0, cv::INTER_CUBIC); /* cv::INTER_LINEAR */
                input.emplace_back(resizeImg.clone());
            }

            if (!baseline_->inference(input)) {
                PRINT_ERROR("Inference Failed.\n");
                return -2;
            }

            float *feat_embedding = baseline_->getOutput();
            TRTASSERT(feat_embedding);
            for (int32_t img_idx = 0; img_idx < static_cast<int32_t>(input.size()); ++img_idx) {
                int32_t output_size = baseline_->getOutputSize();
                cv::Mat feat_mat(1, output_size, CV_32FC1, &feat_embedding[img_idx * output_size]);
                cv::normalize(feat_mat, feat_mat); // 正则化，非常重要，后面求相似矩阵的权值时会用到，否则权值会变成负数
                feats.emplace_back(feat_mat.clone());
            }
        }
        feats_lists_.emplace_back(feats);
    }
    return 0;
}

} // ns_uestc_vhm