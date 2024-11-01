#include "model_service.h"
#include "log.h"
#include "person_reid.h"
#include <memory>

namespace ns_uestc_vhm {

int32_t ObjectTrackService::Init(ModelCfgItem const &cfg) {
    object_track_ctx_ = std::make_shared<ObjectTrackCtx>();
    if (object_track_ctx_.get() == nullptr) {
        PRINT_ERROR("create object_track_ctx_ failed\n");
        return -1;
    }
    object_track_ctx_->SetStrategy(ObjectTrackStrategyFactory::CreateStrategy(cfg.object_track_param.enbaled_strategy_name, cfg));
    return 0;
}

int32_t ObjectTrackService::Track(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists) {
    int32_t ret{0};
    if (object_track_ctx_.get() == nullptr) {
        PRINT_ERROR("object_track_ctx_ is nullptr\n");
        return -1;
    }
    ret = object_track_ctx_->Track(imgs_batch, detect_boxes, feats_lists);
    if (ret < 0) {
        PRINT_ERROR("object_track_ctx_ track failed, ret=%d\n", ret);
        return -2;
    }
    track_boxes_lists_ = object_track_ctx_->GetTrackBoxesLists();
    return 0;
}

std::vector<std::vector<TrackerRes>> ObjectTrackService::GetTrackBoxesLists() const {
    return track_boxes_lists_;
}

int32_t PersonReidService::Init(ModelCfgItem const &cfg) {
    // TODO: add http client and database init, check db init only once
    int32_t ret{0};
    person_reid_ctx_ = std::make_shared<PersonReidCtx>();
    if (person_reid_ctx_.get() == nullptr) {
        PRINT_ERROR("create person_reid_ctx_ failed\n");
        return -1;
    }
    ret = person_reid_ctx_->Init(cfg.person_reid_param);
    if (ret < 0) {
        PRINT_ERROR("init person_reid_ctx_ failed, ret=%d\n", ret);
        return -2;
    }
    return 0;
}

int32_t PersonReidService::Reid(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists) {
    // TODO:
    // traversal detect_boxes, get feats, search in faiss for identify
    // send http request
    int32_t ret{0};
    if (person_reid_ctx_.get() == nullptr) {
        PRINT_ERROR("create person_reid_ctx_ failed\n");
        return -1;
    }
    ret = person_reid_ctx_->Reid(imgs_batch, detect_boxes, feats_lists);
    if (ret < 0) {
        PRINT_ERROR("person_reid_ctx_ reid failed, ret=%d\n", ret);
        return -2;
    }
    reid_boxes_lists_ = person_reid_ctx_->GetReidBoxesLists();
    return 0;
}

std::vector<std::vector<TrackerRes>> PersonReidService::GetReidBoxesLists() const {
    return reid_boxes_lists_;
}

} // ns_uestc_vhm