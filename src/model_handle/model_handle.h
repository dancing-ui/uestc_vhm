#ifndef _UESTC_VHM_MODEL_HANDLE_H_
#define _UESTC_VHM_MODEL_HANDLE_H_

#include "model_manager.h"
#include "model_handle_common.h"
#include "model_service.h"
#include <memory>
namespace ns_uestc_vhm {

class ModelHandle {
public:
    ModelHandle() = default;
    ~ModelHandle();

    ModelHandle(ModelHandle const &) = delete;
    ModelHandle &operator=(ModelHandle const &) = delete;
    ModelHandle(ModelHandle &&) = delete;
    ModelHandle &operator=(ModelHandle &&) = delete;

    int32_t Init(ModelCfgItem const &cfg);
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, ModelHandleCb const &handle_cb);

private:
    std::unique_ptr<ModelManager> model_manager_;
    std::unique_ptr<ObjectTrackService> object_track_service_;
    WorkMode work_mode_;
    ModelHandleRes model_handle_res_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_HANDLE_H_