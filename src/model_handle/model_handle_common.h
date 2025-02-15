#ifndef _UESTC_VHM_MODEL_HANDLE_COMMON_H_
#define _UESTC_VHM_MODEL_HANDLE_COMMON_H_

#include <vector>
#include <functional>
#include <opencv2/opencv.hpp>

namespace ns_uestc_vhm {
struct ClassRes {
    int classes;
    float prob;
};

struct DetectRes : ClassRes {
    float x;
    float y;
    float w;
    float h;
};

struct TrackerRes : public DetectRes {
    TrackerRes(int cl, float pb, float xc, float yc, float wc, float hc, int64_t id) :
        DetectRes() {
        classes = cl, prob = pb, x = xc, y = yc, w = wc, h = hc, object_id = id;
    }
    cv::Mat feature;
    int64_t object_id;
};

struct ObjectTrackRes {
    std::vector<std::vector<TrackerRes>> track_res;
    std::vector<cv::Mat> imgs_batch;
};

struct PersonReidRes {
    std::vector<std::vector<TrackerRes>> track_res;
    std::vector<cv::Mat> imgs_batch;
};

struct ModelHandleRes {
    ObjectTrackRes object_track_res;
    PersonReidRes person_reid_res;
};

using ModelHandleCb = std::function<int32_t(ModelHandleRes const &)>;

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_HANDLE_COMMON_H_