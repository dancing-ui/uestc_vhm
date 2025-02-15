#include "rtmp_handler.h"
#include "log.h"
#include "model_handle_common.h"
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
    model_handle_cb_ = [this](ModelHandleRes const &model_handle_res) -> int32_t {
        return HandleDataOutput(model_handle_res);
    };
    // init stream
    capture_.open(stream_media_cfg.in);
    if (!capture_.isOpened()) {
        PRINT_ERROR("open rtmp's stream failed, stream_id=%d\n", stream_media_cfg.id);
        return -1;
    }
    // init parameter
    ret = InitParameter(stream_media_cfg, model_cfg);
    if (ret < 0) {
        PRINT_ERROR("init rtmp's stream parameter failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
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
    // open output stream
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
    std::vector<cv::Mat> imgs_batch;
    imgs_batch.reserve(model_cfg_.param.batch_size);
    while (is_finished_.load() == false) {
        if (imgs_batch.size() < model_cfg_.param.batch_size) { // get input
            if (capture_.read(frame)) {
                imgs_batch.emplace_back(frame.clone());
            } else {
                static_cast<void>(ReconnectCamera());
                imgs_batch.clear();
                continue;
            }
        } else {
            ret = RawDataInput(imgs_batch);
            if (ret < 0) {
                PRINT_ERROR("RawDataInput failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
                return -1;
            }
            imgs_batch.clear();
        }
    }
    PRINT_INFO("rtmp exiting, stream_id=%d\n", stream_media_cfg_.id);
    return 0;
}

int32_t RtmpHandler::Stop() {
    is_finished_.store(true);
    return 0;
}

int32_t RtmpHandler::InitParameter(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) {
    stream_media_cfg_ = stream_media_cfg;
    model_cfg_ = model_cfg;
    if (model_cfg_.param.num_class == 91) {
        model_cfg_.param.class_names = utils::dataSets::coco91;
    } else if (model_cfg_.param.num_class == 80) {
        model_cfg_.param.class_names = utils::dataSets::coco80;
    } else if (model_cfg_.param.num_class == 20) {
        model_cfg_.param.class_names = utils::dataSets::voc20;
    } else {
        PRINT_ERROR("unsupported num_class=%d, supported num_class=[coco91, coco80, voc20]\n", model_cfg_.param.num_class);
        return -1;
    }
    output_size_.width = stream_media_cfg_.dst_width;
    output_size_.height = stream_media_cfg_.dst_height;
    model_cfg_.param.src_h = capture_.get(cv::CAP_PROP_FRAME_HEIGHT);
    model_cfg_.param.src_w = capture_.get(cv::CAP_PROP_FRAME_WIDTH);
    model_cfg_.person_reid_param.camera_ip = stream_media_cfg_.in;
    return 0;
}

int32_t RtmpHandler::RawDataInput(std::vector<cv::Mat> &imgs_batch) {
    int32_t ret{0};
    if (model_handler_.get() == nullptr) {
        PRINT_ERROR("model_handler_ is nullptr, stream_id=%d\n", stream_media_cfg_.id);
        return -1;
    }
    ret = model_handler_->RawDataInput(imgs_batch, model_handle_cb_);
    if (ret < 0) {
        PRINT_ERROR("RawDataInput failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
        return -2;
    }
    return 0;
}

int32_t RtmpHandler::HandleDataOutput(ModelHandleRes const &model_handle_res) {
    int32_t ret{0};
    switch (model_cfg_.work_mode) {
    case WorkMode::kObjectTrack: {
        cv::Scalar color = cv::Scalar(0, 255, 0);
        cv::Point bbox_points[1][4];
        for (size_t bi = 0; bi < model_handle_res.object_track_res.imgs_batch.size(); bi++) {
            if (!model_handle_res.object_track_res.track_res.empty()) {
                for (auto &box : model_handle_res.object_track_res.track_res[bi]) {
                    if (model_cfg_.param.num_class == 91) {
                        color = utils::Colors::color91[box.classes];
                    } else if (model_cfg_.param.num_class == 80) {
                        color = utils::Colors::color80[box.classes];
                    } else if (model_cfg_.param.num_class == 20) {
                        color = utils::Colors::color80[box.classes];
                    }
                    float left = box.x - box.w / 2;
                    float top = box.y - box.h / 2;
                    cv::rectangle(model_handle_res.object_track_res.imgs_batch[bi], cv::Point(left, top), cv::Point(left + box.w, top + box.h), color, 2, cv::LINE_AA);
                    cv::String det_info = std::to_string(box.object_id);
                    bbox_points[0][0] = cv::Point(left, top);
                    bbox_points[0][1] = cv::Point(left + det_info.size() * 11, top);
                    bbox_points[0][2] = cv::Point(left + det_info.size() * 11, top - 15);
                    bbox_points[0][3] = cv::Point(left, top - 15);
                    std::vector<std::vector<cv::Point>> pts;
                    pts.push_back(std::vector<cv::Point>(bbox_points[0], bbox_points[0] + 4));
                    cv::fillPoly(model_handle_res.object_track_res.imgs_batch[bi], pts, color);
                    cv::putText(model_handle_res.object_track_res.imgs_batch[bi], det_info, bbox_points[0][0], cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
                }
            }
            ret = PushOneFrame(model_handle_res.object_track_res.imgs_batch[bi]);
            if (ret < 0) {
                PRINT_ERROR("push one frame failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
                return -1;
            }
        }
        break;
    }
    case WorkMode::kPersonReid: {
        cv::Scalar color = cv::Scalar(0, 255, 0);
        cv::Point bbox_points[1][4];
        for (size_t bi = 0; bi < model_handle_res.person_reid_res.imgs_batch.size(); bi++) {
            if (!model_handle_res.person_reid_res.track_res.empty()) {
                for (auto &box : model_handle_res.person_reid_res.track_res[bi]) {
                    if (model_cfg_.param.num_class == 91) {
                        color = utils::Colors::color91[box.classes];
                    } else if (model_cfg_.param.num_class == 80) {
                        color = utils::Colors::color80[box.classes];
                    } else if (model_cfg_.param.num_class == 20) {
                        color = utils::Colors::color80[box.classes];
                    }
                    float left = box.x;
                    float top = box.y;
                    cv::rectangle(model_handle_res.person_reid_res.imgs_batch[bi], cv::Point(left, top), cv::Point(left + box.w, top + box.h), color, 2, cv::LINE_AA);
                    cv::String det_info = std::to_string(box.object_id);
                    bbox_points[0][0] = cv::Point(left, top);
                    bbox_points[0][1] = cv::Point(left + det_info.size() * 11, top);
                    bbox_points[0][2] = cv::Point(left + det_info.size() * 11, top - 15);
                    bbox_points[0][3] = cv::Point(left, top - 15);
                    std::vector<std::vector<cv::Point>> pts;
                    pts.push_back(std::vector<cv::Point>(bbox_points[0], bbox_points[0] + 4));
                    cv::fillPoly(model_handle_res.person_reid_res.imgs_batch[bi], pts, color);
                    cv::putText(model_handle_res.person_reid_res.imgs_batch[bi], det_info, bbox_points[0][0], cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
                }
            }
            ret = PushOneFrame(model_handle_res.person_reid_res.imgs_batch[bi]);
            if (ret < 0) {
                PRINT_ERROR("push one frame failed, ret=%d, stream_id=%d\n", ret, stream_media_cfg_.id);
                return -1;
            }
        }
        break;
    }
    case WorkMode::kNoneMode: {
        PRINT_INFO("none work mode, stream_id=%d\n", stream_media_cfg_.id);
        break;
    }
    }
    return 0;
}

int32_t RtmpHandler::PushOneFrame(cv::Mat const &frame) {
    cv::Mat resized_frame;
    cv::resize(frame, resized_frame, output_size_, 0, 0, cv::INTER_LINEAR);
    size_t total_len = resized_frame.total() * resized_frame.elemSize();
    size_t pushed_len = fwrite(resized_frame.data, 1, total_len, fp_);
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