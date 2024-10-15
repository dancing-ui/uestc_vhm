#ifndef _UESTC_VHM_MODEL_MANAGER_H_
#define _UESTC_VHM_MODEL_MANAGER_H_

#include "model_factory.h"
#include "model_handle_common.h"
#include "object_track.h"
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
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, ModelHandleCb const &handle_cb);

private:
    std::shared_ptr<ModelFactory> model_factory_;
    std::shared_ptr<yolo::YOLO> yolo_driver_;
    std::shared_ptr<reid::REID> reid_driver_;
    std::shared_ptr<ObjectTrackCtx> object_track_ctx_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_MANAGER_H_