#include "model_handle.h"
#include "log.h"

namespace ns_uestc_vhm {

ModelHandle::~ModelHandle() {
}

int32_t ModelHandle::Init(ModelCfgItem const &cfg) {
    int32_t ret{0};
    work_mode_ = cfg.work_mode;
    model_manager_ = std::make_unique<ModelManager>();
    if (model_manager_.get() == nullptr) {
        PRINT_ERROR("create model_manager_ failed\n");
        return -1;
    }
    ret = model_manager_->Init(cfg);
    if (ret < 0) {
        PRINT_ERROR("init model_manager_ failed, ret=%d\n", ret);
        return -2;
    }
    switch (work_mode_) {
    case WorkMode::kObjectTrack: {
        object_track_service_ = std::make_unique<ObjectTrackService>();
        if (object_track_service_.get() == nullptr) {
            PRINT_ERROR("create object_track_service_ failed\n");
            return -3;
        }
        ret = object_track_service_->Init(cfg);
        if (ret < 0) {
            PRINT_ERROR("init object_track_service_ failed, ret=%d\n", ret);
            return -4;
        }
        break;
    }
    case WorkMode::kPersonReid: {
        // TODO: add person reid
        break;
    }
    case WorkMode::kNoneMode: {
        PRINT_ERROR("none work mode\n");
        break;
    }
    }
    return 0;
}

int32_t ModelHandle::RawDataInput(std::vector<cv::Mat> &imgs_batch, ModelHandleCb const &handle_cb) {
    int32_t ret{0};
    if (model_manager_.get() == nullptr) {
        PRINT_ERROR("model_manager_ is nullptr\n");
        return -1;
    }
    ret = model_manager_->RawDataInput(imgs_batch);
    if (ret < 0) {
        PRINT_ERROR("model_manager_ RawDataInput failed, ret=%d\n", ret);
        return -2;
    }
    switch (work_mode_) {
    case WorkMode::kObjectTrack: {
        if (object_track_service_.get() == nullptr) {
            PRINT_ERROR("object_track_service_ is nullptr\n");
            return -3;
        }
        ret = object_track_service_->Track(imgs_batch, model_manager_->GetDetectBoxes(), model_manager_->GetFeatsLists());
        if (ret < 0) {
            PRINT_ERROR("object_track_ctx_ track failed, ret=%d\n", ret);
            return -4;
        }
        model_handle_res_.object_track_res.imgs_batch = imgs_batch;
        model_handle_res_.object_track_res.track_res = object_track_service_->GetTrackBoxesLists();
        break;
    }
    case WorkMode::kPersonReid: {
        // TODO: add person reid
        break;
    }
    case WorkMode::kNoneMode: {
        PRINT_ERROR("none work mode\n");
        break;
    }
    }

    if (handle_cb != nullptr) {
        ret = handle_cb(model_handle_res_);
        if (ret < 0) {
            return -5;
        }
    }
    return 0;
}

} // ns_uestc_vhm
