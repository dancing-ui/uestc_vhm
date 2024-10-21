#ifndef _UESTC_VHM_MODEL_MANAGER_H_
#define _UESTC_VHM_MODEL_MANAGER_H_

#include "model_factory.h"
#include "parameter.h"
#include <memory>

namespace ns_uestc_vhm {

class ModelManager {
public:
    ModelManager() = default;
    ~ModelManager();

    ModelManager(ModelManager const &) = delete;
    ModelManager &operator=(ModelManager const &) = delete;
    ModelManager(ModelManager &&) = delete;
    ModelManager &operator=(ModelManager &&) = delete;

    int32_t Init(ModelCfgItem const &cfg);
    
    int32_t ObjectDetectInput(std::vector<cv::Mat> &imgs_batch);
    int32_t FeatureExtractInput(std::vector<cv::Mat> &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes);

    std::vector<std::vector<utils::Box>> GetDetectBoxes();
    std::vector<std::vector<cv::Mat>> GetFeatsLists();

    void Reset();

private:
    std::shared_ptr<ModelFactory> model_factory_;
    std::shared_ptr<yolo::YOLO> yolo_driver_;
    std::shared_ptr<reid::REID> reid_driver_;

    std::vector<std::vector<utils::Box>> detect_boxes_;
    std::vector<std::vector<cv::Mat>> feats_lists_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_MANAGER_H_