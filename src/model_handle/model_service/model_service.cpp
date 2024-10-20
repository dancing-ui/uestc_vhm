#include "model_service.h"
#include "log.h"

namespace ns_uestc_vhm {

ObjectTrackService::~ObjectTrackService() {
}

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

std::vector<std::vector<TrackerRes>> ObjectTrackService::GetTrackBoxesLists() {
    return track_boxes_lists_;
}

} // ns_uestc_vhm