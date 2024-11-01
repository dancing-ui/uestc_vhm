#ifndef _UESTC_VHM_RTMP_PUSHER_H_
#define _UESTC_VHM_RTMP_PUSHER_H_

#include <atomic>

#include "stream_media_driver.h"
#include "model_handle.h"

namespace ns_uestc_vhm {

class RtmpHandler : public StreamMediaDriver {
public:
    RtmpHandler() = default;
    ~RtmpHandler();

    RtmpHandler(RtmpHandler const &) = delete;
    RtmpHandler &operator=(RtmpHandler const &) = delete;
    RtmpHandler(RtmpHandler &&) = delete;
    RtmpHandler &operator=(RtmpHandler &&) = delete;

    int32_t Init(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) override;
    int32_t Start() override;
    int32_t Stop() override;
    int32_t InitParameter(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg);
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch);
    int32_t HandleDataOutput(ModelHandleRes const &model_handle_res);
    int32_t PushOneFrame(cv::Mat const &frame);

private:
    int32_t ReconnectCamera();
    int32_t OpenOutputStream();

    utils::InputStream source_{utils::InputStream::CAMERA};
    std::string video_path_;
    std::string image_path_;
    int32_t camera_id_{0};
    int total_batches_{0};
    int delay_time_{0};
    cv::Size output_size_;

    cv::VideoCapture capture_;

    StreamMediaCfgItem stream_media_cfg_;
    ModelCfgItem model_cfg_;

    std::unique_ptr<ModelHandle> model_handler_; // TODO：move model to stream media driver class for reuse
    FILE *fp_{nullptr};

    ModelHandleCb model_handle_cb_;

    std::atomic<bool> is_finished_{false};
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_RTMP_PUSHER_H_