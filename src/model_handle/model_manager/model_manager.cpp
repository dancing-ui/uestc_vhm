#include "model_manager.h"
#include "log.h"

namespace ns_uestc_vhm {

ModelManager::~ModelManager() {
}

int32_t ModelManager::Init(ModelCfgItem const &cfg) {
    int32_t ret{0};
    model_factory_ = std::make_shared<YoloFactory>();
    if (model_factory_.get() == nullptr) {
        PRINT_ERROR("create model_factory_ failed\n");
        return -1;
    }
    yolo_driver_ = model_factory_->CreateYoloObj(cfg);
    if (yolo_driver_.get() == nullptr) {
        PRINT_ERROR("create yolo_driver_ failed\n");
        return -1;
    }
    ret = yolo_driver_->Init();
    if (ret < 0) {
        PRINT_ERROR("init yolo_driver_ failed, ret=%d\n", ret);
        return -1;
    }
    return 0;
}

int32_t ModelManager::RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batch_nums, ModelHandleCb const &handle_cb) {
    return yolo_driver_->RawDataInput(imgs_batch, batch_nums, handle_cb);
}

} // ns_uestc_vhm
