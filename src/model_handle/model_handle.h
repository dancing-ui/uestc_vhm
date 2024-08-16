#ifndef _UESTC_VHM_MODEL_HANDLE_H_
#define _UESTC_VHM_MODEL_HANDLE_H_

#include "model_manager.h"
#include "model_handle_common.h"
namespace ns_uestc_vhm {

class ModelHandle {
public:
    ModelHandle() = default;
    ~ModelHandle();

    ModelHandle(ModelHandle const &) = default;
    ModelHandle &operator=(ModelHandle const &) = default;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batch_nums, ModelHandleCb const &handle_cb);

private:
    std::unique_ptr<ModelManager> model_manager_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_HANDLE_H_