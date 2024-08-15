#ifndef _UESTC_VHM_RTMP_PUSHER_H_
#define _UESTC_VHM_RTMP_PUSHER_H_

#include <atomic>

#include "stream_media_driver.h"
#include "common_include.h"
#include "model_handle.h"

namespace ns_uestc_vhm {

class RtmpHandler : public StreamMediaDriver {
public:
    RtmpHandler() = default;
    ~RtmpHandler();

    RtmpHandler(RtmpHandler const &) = default;
    RtmpHandler &operator=(RtmpHandler const &) = default;

    int32_t Init(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) override;
    int32_t Start() override;
    int32_t Stop() override;
    void InitModelParameters(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg);
    int32_t RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batchi);
    int32_t HandledDataOutput(std::vector<std::vector<utils::Box>> const &objectss,
                             std::vector<std::string> const &classNames,
                             std::vector<cv::Mat> const &imgsBatch);
    int32_t PushOneFrame(cv::Mat const &frame);

    // int32_t Init(std::string const &output_stream_url, cv::VideoCapture const &input_stream);

    // void Handle(const std::vector<std::vector<utils::Box>> &objectss, const std::vector<std::string> &classNames,
    //             const int &cvDelayTime, std::vector<cv::Mat> &imgsBatch);
    // int32_t PushOneFrame(cv::Mat const &dat);
    // std::string GetUrl() const;

private:
    utils::InputStream source_{utils::InputStream::CAMERA};
    std::string video_path_;
    std::string image_path_;
    int32_t camera_id_{0};
    int total_batches_{0};
    int delay_time_{1};

    cv::VideoCapture capture_;

    StreamMediaCfgItem stream_media_cfg_;
    ModelCfgItem model_cfg_;

    std::unique_ptr<ModelHandle> model_handler_;
    std::string rtmp_url_{""};
    FILE *fp_{nullptr};

    ModelHandleCb target_data_cb_;

    std::atomic<bool> is_finished_{false};

    uint64_t handled_batch_nums_{0};
    // int32_t stream_width{1920};
    // int32_t stream_height{1080};
    // int32_t stream_fps{30};
    // // Open output stream
    // AVFormatContext *fmt_ctx{nullptr};
    // // Add video stream
    // AVStream *video_stream{nullptr};
    // // Find video encoder
    // AVCodec *codec{nullptr};
    // // Open video encoder
    // AVCodecContext *codec_ctx{nullptr};
    // // Presentation timestamp
    // int64_t pts{0};

    // bool ReadAVPixelFormatId(cv::Mat const &mat, AVPixelFormat &format);
    // std::shared_ptr<AVFrame> ConvertToAVFrame(cv::Mat const &mat, AVPixelFormat const &target_format);
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_RTMP_PUSHER_H_