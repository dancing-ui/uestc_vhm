#ifndef _UESTC_VHM_OBJECT_TRACK_H_
#define _UESTC_VHM_OBJECT_TRACK_H_

#include "utils.h"
#include "model_handle_common.h"
#include <memory>
#include <opencv2/core/mat.hpp>

namespace ns_uestc_vhm {

class ObjectTrackStrategy {
public:
    virtual int32_t Init() = 0;
    virtual int32_t Track(std::vector<utils::Box> const &detect_boxes, std::vector<cv::Mat> const &feats, int32_t width, int32_t height) = 0;
    virtual std::vector<TrackerRes> GetTrackBoxes() = 0;
};

class ObjectTrackCtx {
public:
    ObjectTrackCtx() = default;
    ~ObjectTrackCtx() = default;

    ObjectTrackCtx(ObjectTrackCtx const &) = delete;
    ObjectTrackCtx &operator=(ObjectTrackCtx const &) = delete;
    ObjectTrackCtx(ObjectTrackCtx &&) = delete;
    ObjectTrackCtx &operator=(ObjectTrackCtx &&) = delete;

    void SetStrategy(std::shared_ptr<ObjectTrackStrategy> strategy);

    int32_t Track(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists);
    std::vector<std::vector<TrackerRes>> BatchGetTrackBoxes();

public:
    std::shared_ptr<ObjectTrackStrategy> strategy_;

private:
    std::vector<std::vector<TrackerRes>> track_boxes_lists_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_OBJECT_TRACK_H_