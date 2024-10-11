#include "model_handle.h"
#include "log.h"

namespace ns_uestc_vhm {

ModelHandle::~ModelHandle() {
}

int32_t ModelHandle::Init(ModelCfgItem const &cfg) {
    int32_t ret{0};
    model_manager_ = std::make_unique<ModelManager>();
    if (model_manager_.get() == nullptr) {
        PRINT_ERROR("create model_manager_ failed\n");
        return -1;
    }
    ret = model_manager_->Init(cfg);
    if (ret < 0) {
        PRINT_ERROR("init model_manager_ failed, ret=%d\n", ret);
        return -2;
    }
    return 0;
}

int32_t ModelHandle::RawDataInput(std::vector<cv::Mat> &imgs_batch, ModelHandleCb const &handle_cb) {
    return model_manager_->RawDataInput(imgs_batch, handle_cb);
}

} // ns_uestc_vhm
