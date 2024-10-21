#include "model_manager.h"
#include "log.h"
#include <memory>

namespace ns_uestc_vhm {

ModelManager::~ModelManager() {
}

int32_t ModelManager::Init(ModelCfgItem const &cfg) {
    int32_t ret{0};
    model_factory_ = std::make_shared<YoloFactory>();
    if (model_factory_.get() == nullptr) {
        PRINT_ERROR("create model_factory_ failed\n");
        return -1;
    }
    yolo_driver_ = model_factory_->CreateYoloObj(cfg);
    if (yolo_driver_.get() == nullptr) {
        PRINT_ERROR("create yolo_driver_ failed\n");
        return -2;
    }
    ret = yolo_driver_->Init();
    if (ret < 0) {
        PRINT_ERROR("init yolo_driver_ failed, ret=%d\n", ret);
        return -3;
    }

    model_factory_ = std::make_shared<ReidFactory>();
    if (model_factory_.get() == nullptr) {
        PRINT_ERROR("create model_factory_ failed\n");
        return -4;
    }
    reid_driver_ = model_factory_->CreateReidObj(cfg);
    if (reid_driver_.get() == nullptr) {
        PRINT_ERROR("create reid_driver_ failed\n");
        return -5;
    }
    ret = reid_driver_->Init();
    if (ret < 0) {
        PRINT_ERROR("init reid_driver_ failed, ret=%d\n", ret);
        return -6;
    }
    return 0;
}

int32_t ModelManager::ObjectDetectInput(std::vector<cv::Mat> &imgs_batch) {
    int32_t ret{0};
    if (yolo_driver_.get() == nullptr) {
        PRINT_ERROR("yolo_driver_ is nullptr\n");
        return -1;
    }
    ret = yolo_driver_->BatchInference(imgs_batch);
    if (ret < 0) {
        PRINT_ERROR("yolo_driver_ BatchInference failed, ret=%d\n", ret);
        return -2;
    }
    detect_boxes_ = yolo_driver_->getObjectss();
    return 0;
}

int32_t ModelManager::FeatureExtractInput(std::vector<cv::Mat> &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes) {
    int32_t ret{0};
    if (reid_driver_.get() == nullptr) {
        PRINT_ERROR("reid_driver_ is nullptr\n");
        return -1;
    }
    ret = reid_driver_->BatchInference(imgs_batch, detect_boxes);
    if (ret < 0) {
        PRINT_ERROR("reid_driver_ BatchInference failed, ret=%d\n", ret);
        return -2;
    }
    feats_lists_ = reid_driver_->GetFeatLists();
    return 0;
}

std::vector<std::vector<utils::Box>> ModelManager::GetDetectBoxes() {
    return detect_boxes_;
}

std::vector<std::vector<cv::Mat>> ModelManager::GetFeatsLists() {
    return feats_lists_;
}

void ModelManager::Reset() {
    if (yolo_driver_.get() != nullptr) {
        yolo_driver_->reset();
    }
}

} // ns_uestc_vhm
