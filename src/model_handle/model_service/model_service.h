#ifndef _UESTC_VHM_MODEL_SERVICE_H_
#define _UESTC_VHM_MODEL_SERVICE_H_

#include "parameter.h"
#include "object_track.h"
#include "person_reid.h"
#include <cstdint>
#include <memory>

namespace ns_uestc_vhm {

class ObjectTrackService {
public:
    ObjectTrackService() = default;
    ~ObjectTrackService() = default;

    ObjectTrackService(ObjectTrackService const &) = delete;
    ObjectTrackService &operator=(ObjectTrackService const &) = delete;
    ObjectTrackService(ObjectTrackService &&) = delete;
    ObjectTrackService &operator=(ObjectTrackService &&) = delete;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t Track(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists);
    std::vector<std::vector<TrackerRes>> GetTrackBoxesLists() const;

private:
    std::shared_ptr<ObjectTrackCtx> object_track_ctx_;
    std::vector<std::vector<TrackerRes>> track_boxes_lists_;
};

class PersonReidService {
public:
    PersonReidService() = default;
    ~PersonReidService() = default;

    PersonReidService(PersonReidService const &) = delete;
    PersonReidService &operator=(PersonReidService const &) = delete;
    PersonReidService(PersonReidService &&) = delete;
    PersonReidService &operator=(PersonReidService &&) = delete;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t Reid(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists);
    std::vector<std::vector<TrackerRes>> GetReidBoxesLists() const;

private:
    std::shared_ptr<PersonReidCtx> person_reid_ctx_;
    std::vector<std::vector<TrackerRes>> reid_boxes_lists_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_SERVICE_H_