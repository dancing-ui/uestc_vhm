#ifndef _UESTC_VHM_RTMP_PUSHER_H_
#define _UESTC_VHM_RTMP_PUSHER_H_

namespace ns_uestc_vhm {
class RtmpPusher {
public:
    RtmpPusher() = default;
    ~RtmpPusher();
    int32_t Init(std::string const &output_stream_url, cv::VideoCapture const &input_stream);
    void Handle(const std::vector<std::vector<utils::Box>> &objectss, const std::vector<std::string> &classNames,
                const int &cvDelayTime, std::vector<cv::Mat> &imgsBatch);
    int32_t PushOneFrame(cv::Mat const &dat);
    std::string GetUrl() const;

private:
    std::string rtmp_url_{""};
    FILE *fp_{nullptr};
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