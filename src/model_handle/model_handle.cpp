#include "model_handle.h"
#include "log.h"
#include "model_service.h"
#include "utils.h"

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
        person_reid_service_ = std::make_unique<PersonReidService>();
        if (person_reid_service_.get() == nullptr) {
            PRINT_ERROR("create person_reid_service_ failed\n");
            return -5;
        }
        ret = person_reid_service_->Init(cfg);
        if (ret < 0) {
            PRINT_ERROR("init person_reid_service_ failed, ret=%d\n", ret);
            return -6;
        }
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
    TIMERSTART(object_detect);
    ret = model_manager_->ObjectDetectInput(imgs_batch);
    TIMEREND(object_detect);
    DURATION_ms(object_detect);
    if (ret < 0) {
        PRINT_ERROR("model_manager_ ObjectDetectInput failed, ret=%d\n", ret);
        return -2;
    }

    auto detect_boxes = model_manager_->GetDetectBoxes();
    TIMERSTART(feature_extract);
    ret = model_manager_->FeatureExtractInput(imgs_batch, detect_boxes);
    TIMEREND(feature_extract);
    DURATION_ms(feature_extract);
    if (ret < 0) {
        PRINT_ERROR("model_manager_ FeatureExtractInput failed, ret=%d\n", ret);
        return -3;
    }
    auto feats_lists = model_manager_->GetFeatsLists();

    switch (work_mode_) {
    case WorkMode::kObjectTrack: {
        if (object_track_service_.get() == nullptr) {
            PRINT_ERROR("object_track_service_ is nullptr\n");
            return -4;
        }
        TIMERSTART(object_track);
        ret = object_track_service_->Track(imgs_batch, detect_boxes, feats_lists);
        TIMEREND(object_track);
        DURATION_ms(object_track);
        if (ret < 0) {
            PRINT_ERROR("object_track_ctx_ track failed, ret=%d\n", ret);
            return -5;
        }
        model_handle_res_.object_track_res.imgs_batch = imgs_batch;
        model_handle_res_.object_track_res.track_res = object_track_service_->GetTrackBoxesLists();
        model_manager_->Reset(); // important
        break;
    }
    case WorkMode::kPersonReid: {
        if (person_reid_service_.get() == nullptr) {
            PRINT_ERROR("person_reid_service_ is nullptr\n");
            return -6;
        }
        TIMERSTART(person_reid);
        ret = person_reid_service_->Reid(imgs_batch, detect_boxes, feats_lists);
        TIMEREND(person_reid);
        DURATION_ms(person_reid);
        if (ret < 0) {
            PRINT_ERROR("person_reid_service_ reid failed, ret=%d\n", ret);
            return -7;
        }
        model_handle_res_.person_reid_res.imgs_batch = imgs_batch;
        model_handle_res_.person_reid_res.track_res = person_reid_service_->GetReidBoxesLists();
        model_manager_->Reset(); // important
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
            return -8;
        }
    }
    return 0;
}

} // ns_uestc_vhm
