#include "object_track.h"
#include "log.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace ns_uestc_vhm {

void ObjectTrackCtx::SetStrategy(std::shared_ptr<ObjectTrackStrategy> strategy) {
    this->strategy_ = strategy;
}

int32_t ObjectTrackCtx::Track(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists) {
    if (strategy_.get() == nullptr) {
        PRINT_ERROR("ObjectTrackCtx::Track: strategy is null\n");
        return -1;
    }
    int32_t ret{0};
    assert(detect_boxes.size() == feats_lists.size());
    assert(imgs_batch.size() == feats_lists.size());
    track_boxes_lists_.clear();
    for (size_t i = 0; i < detect_boxes.size(); i++) {
        ret = strategy_->Track(detect_boxes[i], feats_lists[i], imgs_batch[i].cols, imgs_batch[i].rows);
        if (ret < 0) {
            PRINT_ERROR("ObjectTrackCtx::Track: strategy_->Track failed, ret=%d\n", ret);
            return ret;
        }
        track_boxes_lists_.emplace_back(strategy_->GetTrackBoxes());
    }
    return 0;
}

std::vector<std::vector<TrackerRes>> ObjectTrackCtx::BatchGetTrackBoxes() {
    return track_boxes_lists_;
}

} // ns_uestc_vhm