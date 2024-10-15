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

    object_track_ctx_ = std::make_shared<ObjectTrackCtx>();
    if (object_track_ctx_.get() == nullptr) {
        PRINT_ERROR("create object_track_ctx_ failed\n");
        return -9;
    }
    object_track_ctx_->SetStrategy(ObjectTrackStrategyFactory::CreateStrategy(cfg.object_track_param.enbaled_strategy_name, cfg));

    return 0;
}

int32_t ModelManager::RawDataInput(std::vector<cv::Mat> &imgs_batch, ModelHandleCb const &handle_cb) {
    int32_t ret{0};
    if (yolo_driver_.get() == nullptr) {
        PRINT_ERROR("yolo_driver_ is nullptr\n");
        return -1;
    }

    if (reid_driver_.get() == nullptr) {
        PRINT_ERROR("reid_driver_ is nullptr\n");
        return -2;
    }

    if (object_track_ctx_.get() == nullptr) {
        PRINT_ERROR("object_track_ctx_ is nullptr\n");
        return -3;
    }

    auto start_all = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    ret = yolo_driver_->BatchInference(imgs_batch);
    if (ret < 0) {
        PRINT_ERROR("yolo_driver_ BatchInference failed, ret=%d\n", ret);
        return -4;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    PRINT_INFO("yolo_driver_ BatchInference cost %ld ms\n", duration.count());

    auto detect_boxes = yolo_driver_->getObjectss();

    start = std::chrono::high_resolution_clock::now();
    ret = reid_driver_->BatchInference(imgs_batch, detect_boxes);
    if (ret < 0) {
        PRINT_ERROR("reid_driver_ BatchInference failed, ret=%d\n", ret);
        return -5;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    PRINT_INFO("reid_driver_ BatchInference cost %ld ms\n", duration.count());

    auto object_features = reid_driver_->GetFeatLists();

    start = std::chrono::high_resolution_clock::now();
    ret = object_track_ctx_->Track(imgs_batch, detect_boxes, object_features);
    if (ret < 0) {
        PRINT_ERROR("object_track_ctx_ track failed, ret=%d\n", ret);
        return -6;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    PRINT_INFO("object_track_ctx_ track cost %ld ms\n", duration.count());

    auto track_boxes_lists = object_track_ctx_->BatchGetTrackBoxes();

    auto end_all = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_all - start_all);
    PRINT_INFO("ModelManager RawDataInput cost %ld ms\n", duration.count());

    auto start_handle = std::chrono::high_resolution_clock::now();
    if (handle_cb != nullptr) {
        ret = handle_cb(track_boxes_lists, imgs_batch);
        if (ret < 0) {
            return -7;
        }
    }
    auto end_handle = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_handle - start_handle);
    PRINT_INFO("ModelManager handle cost %ld ms\n", duration.count());

    yolo_driver_->reset();
    return 0;
}

} // ns_uestc_vhm
