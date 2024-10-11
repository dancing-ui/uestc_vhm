#ifndef _UESTC_VHM_OBJECT_TRACKER_TRACKER_H_
#define _UESTC_VHM_OBJECT_TRACKER_TRACKER_H_

#include "KalmanTracker.h"
#include "object_track.h"
#include "parameter.h"
#include "model_handle_common.h"

#include <set>
#include <vector>
#include <map>

namespace ns_uestc_vhm {

class ObjectTracker : public ObjectTrackStrategy {
public:
    explicit ObjectTracker(ModelCfgItem const &cfg);
    virtual ~ObjectTracker();

    ObjectTracker(ObjectTracker const &) = delete;
    ObjectTracker &operator=(ObjectTracker const &) = delete;
    ObjectTracker(ObjectTracker &&) = delete;
    ObjectTracker &operator=(ObjectTracker &&) = delete;

    void update(const std::vector<DetectRes> &det_boxes, const std::vector<cv::Mat> &det_features,
                int width, int height);

    int32_t Init() override;
    int32_t Track(std::vector<utils::Box> const &detect_boxes, std::vector<cv::Mat> const &feats, int32_t width, int32_t height) override;
    std::vector<TrackerRes> GetTrackBoxes() override;

public:
    std::vector<TrackerRes> tracker_boxes;

    ModelCfgItem cfg_;

private:
    static float IOUCalculate(const TrackerRes &det_a, const TrackerRes &det_b);
    void Alignment(std::vector<std::vector<double>> mat, std::set<int> &unmatchedDetections,
                   std::set<int> &unmatchedTrajectories, std::vector<cv::Point> &matchedPairs,
                   int det_num, int trk_num, bool b_iou);
    void FeatureMatching(const std::vector<TrackerRes> &predict_boxes, std::set<int> &unmatchedDetections,
                         std::set<int> &unmatchedTrajectories, std::vector<cv::Point> &matchedPairs);
    void IOUMatching(const std::vector<TrackerRes> &predict_boxes, std::set<int> &unmatchedDetections,
                     std::set<int> &unmatchedTrajectories, std::vector<cv::Point> &matchedPairs);
    int max_age;
    float iou_threshold;
    float sim_threshold;
    bool agnostic;
    std::vector<KalmanTracker> kalman_boxes;
    std::map<int, std::string> class_labels;
    std::string labels_file;
    std::vector<cv::Scalar> id_colors;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_OBJECT_TRACKER_TRACKER_H_
