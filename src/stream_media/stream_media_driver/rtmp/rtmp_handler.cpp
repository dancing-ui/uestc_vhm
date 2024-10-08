#include "rtmp_handler.h"
#include "log.h"
#include "utils.h"

namespace ns_uestc_vhm {

RtmpHandler::~RtmpHandler() {
    if (fp_ != nullptr) {
        pclose(fp_);
    }
    if (capture_.isOpened()) {
        capture_.release();
    }
}
int32_t RtmpHandler::Init(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) {
    int32_t ret{0};
    target_data_cb_ = [this](std::vector<std::vector<utils::Box>> const &objectss,
                             std::vector<std::string> const &classNames,
                             std::vector<cv::Mat> const &imgsBatch) -> int32_t {
        return HandledDataOutput(objectss, classNames, imgsBatch);
    };
    InitModelParameters(stream_media_cfg, model_cfg);
    capture_.open(stream_media_cfg_.in);
    if (!capture_.isOpened()) {
        PRINT_ERROR("open rtmp's stream failed, stream_id=%d\n", stream_media_cfg_.id);
        return -1;
    }
    if (!utils::setInputStream(source_, image_path_, video_path_, camera_id_,
                               capture_, total_batches_, delay_time_, model_cfg_.param)) {
        PRINT_ERROR("set rtmp's stream failed, stream_id=%d\n", stream_media_cfg_.id);
        return -2;
    }
    // init model
    model_handler_ = std::make_unique<ModelHandle>();
    if (model_handler_.get() == nullptr) {
        PRINT_ERROR("create model_handler_ failed, stream_id=%d\n", stream_media_cfg_.id);
        return -3;
    }
    ret = model_handler_->Init(model_cfg_);
    if (ret < 0) {
        PRINT_ERROR("init model_handler_ failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
        return -4;
    }
    ret = OpenOutputStream();
    if (ret < 0) {
        PRINT_ERROR("open rtmp's stream failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
        return -5;
    }
    return 0;
}

int32_t RtmpHandler::Start() {
    int32_t ret{0};
    cv::Mat frame;
    cv::Mat resized_frame;
    std::vector<cv::Mat> imgs_batch;
    imgs_batch.reserve(model_cfg_.param.batch_size);
    uint64_t batch_nums = 0;
    while (is_finished_.load() == false) {
        if (imgs_batch.size() < model_cfg_.param.batch_size) { // get input
            if (capture_.read(frame)) {
                cv::resize(frame, resized_frame, output_size_, 0, 0, cv::INTER_LINEAR);
                imgs_batch.emplace_back(resized_frame.clone());
            } else {
                static_cast<void>(ReconnectCamera());
                imgs_batch.clear();
                batch_nums = 0;
                continue;
            }
        } else {
            ret = RawDataInput(imgs_batch, batch_nums);
            if (ret < 0) {
                PRINT_ERROR("RawDataInput failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
                return -1;
            }
            imgs_batch.clear();
            batch_nums++;
        }
    }
    PRINT_INFO("rtmp exiting, stream_id=%d\n", stream_media_cfg_.id);
    return 0;
}

int32_t RtmpHandler::Stop() {
    is_finished_.store(true);
    return 0;
}

void RtmpHandler::InitModelParameters(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) {
    stream_media_cfg_ = stream_media_cfg;
    model_cfg_ = model_cfg;
    model_cfg_.param.class_names = utils::dataSets::coco80;
    // model_cfg_.param.class_names = utils::dataSets::voc20;
    model_cfg_.param.num_class = 80; // for coco
    // model_cfg_.param.num_class = 20; // for voc2012
    model_cfg_.param.input_output_names = {"images", "output0"};
    // model_cfg_.param.conf_thresh = 0.25f;
    // model_cfg_.param.iou_thresh = 0.45f;
    model_cfg_.param.src_w = output_size_.width = stream_media_cfg_.dst_width;
    model_cfg_.param.src_h = output_size_.height = stream_media_cfg_.dst_height;
}

int32_t RtmpHandler::RawDataInput(std::vector<cv::Mat> &imgs_batch, int32_t const &batch_nums) {
    int32_t ret{0};
    ret = model_handler_->RawDataInput(imgs_batch, batch_nums, target_data_cb_);
    if (ret < 0) {
        PRINT_ERROR("RawDataInput failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
        return -1;
    }
    return 0;
}

int32_t RtmpHandler::HandledDataOutput(std::vector<std::vector<utils::Box>> const &objectss,
                                       std::vector<std::string> const &classNames,
                                       std::vector<cv::Mat> const &imgsBatch) {
    int32_t ret{0};
    // add rectangle for objectss
    cv::Scalar color = cv::Scalar(0, 255, 0);
    cv::Point bbox_points[1][4];
    const cv::Point *bbox_point0[1] = {bbox_points[0]};
    int num_points[] = {4};
    for (size_t bi = 0; bi < imgsBatch.size(); bi++) {
        if (!objectss.empty()) {
            for (auto &box : objectss[bi]) {
                if (classNames.size() == 91) // coco91
                {
                    color = utils::Colors::color91[box.label];
                }
                if (classNames.size() == 80) // coco80
                {
                    color = utils::Colors::color80[box.label];
                }
                if (classNames.size() == 20) // voc20
                {
                    color = utils::Colors::color80[box.label];
                }
                cv::rectangle(imgsBatch[bi], cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), color, 2, cv::LINE_AA);
                cv::String det_info = classNames[box.label] + " " + cv::format("%.4f", box.confidence);
                bbox_points[0][0] = cv::Point(box.left, box.top);
                bbox_points[0][1] = cv::Point(box.left + det_info.size() * 11, box.top);
                bbox_points[0][2] = cv::Point(box.left + det_info.size() * 11, box.top - 15);
                bbox_points[0][3] = cv::Point(box.left, box.top - 15);
                cv::fillPoly(imgsBatch[bi], bbox_point0, num_points, 1, color);
                cv::putText(imgsBatch[bi], det_info, bbox_points[0][0], cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

                if (!box.land_marks.empty()) // for facial landmarks
                {
                    for (auto &pt : box.land_marks) {
                        cv::circle(imgsBatch[bi], pt, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA, 0);
                    }
                }
            }
        }
        ret = PushOneFrame(imgsBatch[bi]);
        if (ret < 0) {
            PRINT_ERROR("push one frame failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
            return -1;
        }
    }
    return 0;
}

int32_t RtmpHandler::PushOneFrame(cv::Mat const &frame) {
    size_t total_len = frame.total() * frame.elemSize();
    size_t pushed_len = fwrite(frame.data, 1, total_len, fp_);
    fflush(fp_);
    if (pushed_len != total_len) {
        PRINT_ERROR("push one frame incompleted, total_len=%lu, pushed_len=%lu, stream_id=%d\n", total_len, pushed_len, stream_media_cfg_.id);
        return -1;
    }
    return 0;
}

int32_t RtmpHandler::ReconnectCamera() {
    int32_t ret{0};
    uint64_t retry_nums{0};
    cv::Mat frame;
    PRINT_INFO("trying to reconnect %s, stream_id=%d\n", stream_media_cfg_.in.c_str(), stream_media_cfg_.id);
    while (is_finished_.load() == false) {
        capture_.open(stream_media_cfg_.in);
        if (capture_.read(frame)) {
            break;
        }
        retry_nums++;
        PRINT_INFO("trying to reconnect %s, retry_nums=%lu, stream_id=%d\n", stream_media_cfg_.in.c_str(), retry_nums, stream_media_cfg_.id);
        sleep(5);
    }
    if (is_finished_.load() == true) {
        return 0;
    }
    PRINT_INFO("reconnecting to %s succeeded, total_retry_nums=%lu, stream_id=%d\n", stream_media_cfg_.in.c_str(), retry_nums, stream_media_cfg_.id);
    return 0;
}

int32_t RtmpHandler::OpenOutputStream() {
    int32_t ret{0};
    PRINT_INFO("trying to open output stream %s, stream_id=%d\n", stream_media_cfg_.out.c_str(), stream_media_cfg_.id);
    if (fp_ != nullptr) {
        ret = pclose(fp_);
        if (ret < 0) {
            PRINT_ERROR("pclose failed, ret=%d\n", ret);
            return -1;
        }
        fp_ = nullptr;
    }
    std::stringstream command;
    command << "ffmpeg ";
    // infile options
    command << "-y "               // overwrite output files
            << "-an "              // close the audio output
            << "-hide_banner "     // close the ffmpeg's version info
            << "-f rawvideo "      // force format to rawvideo
            << "-vcodec rawvideo " // force video rawvideo ('copy' to copy stream)
            << "-pix_fmt bgr24 "   // set pixel format to bgr24
            << "-s "               // set frame size (WxH or abbreviation)
            << std::to_string(output_size_.width)
            << "x"
            << std::to_string(output_size_.height)
            << " -r  " // set frame rate (Hz value, fraction or abbreviation)
            << std::to_string(int(capture_.get(5)));
    command << " -i - "; //

    // outfile options
    command << "-vcodec libx264 "   // Hyper fast Audio and Video encoder
            << " -pix_fmt yuv420p " // set pixel format to yuv420p
            //<< "-preset ultrafast " // set the libx264 encoding preset to ultrafast
            << "-f flv " // force format to flv
                         //   << " -flvflags no_duration_filesize "
            << stream_media_cfg_.out;
    fp_ = popen(command.str().c_str(), "w");
    if (fp_ == nullptr) {
        PRINT_ERROR("open rtmp's stream failed, cmd=%s, stream_id=%d\n", command.str().c_str(), stream_media_cfg_.id);
        return -2;
    }
    PRINT_INFO("open output stream %s succeeded, stream_id=%d\n", stream_media_cfg_.out.c_str(), stream_media_cfg_.id);
    return 0;
}

} // ns_uestc_vhm