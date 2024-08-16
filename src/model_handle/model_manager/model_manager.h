#ifndef _UESTC_VHM_MODEL_MANAGER_H_
#define _UESTC_VHM_MODEL_MANAGER_H_

#include "model_factory.h"
#include "model_handle_common.h"

namespace ns_uestc_vhm {
class ModelManager {
public:
    ModelManager() = default;
    ~ModelManager();

    ModelManager(ModelManager const &) = default;
    ModelManager &operator=(ModelManager const &) = default;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batch_nums, ModelHandleCb const &handle_cb);
    // int32_t Start();
    // int32_t Stop();
    // int32_t Handle();

private:
    std::shared_ptr<ModelFactory> model_factory_;
    std::shared_ptr<yolo::YOLO> yolo_driver_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_MANAGER_H_