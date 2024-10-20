#ifndef _UESTC_VHM_MODEL_SERVICE_H_
#define _UESTC_VHM_MODEL_SERVICE_H_

#include "parameter.h"
#include "object_track.h"
#include <cstdint>

namespace ns_uestc_vhm {

class ObjectTrackService {
public:
    ObjectTrackService() = default;
    ~ObjectTrackService();

    ObjectTrackService(ObjectTrackService const &) = delete;
    ObjectTrackService &operator=(ObjectTrackService const &) = delete;
    ObjectTrackService(ObjectTrackService &&) = delete;
    ObjectTrackService &operator=(ObjectTrackService &&) = delete;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t Track(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists);
    std::vector<std::vector<TrackerRes>> GetTrackBoxesLists();

private:
    std::shared_ptr<ObjectTrackCtx> object_track_ctx_;
    std::vector<std::vector<TrackerRes>> track_boxes_lists_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_SERVICE_H_