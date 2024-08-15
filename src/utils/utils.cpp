#include "utils.h"

namespace ns_uestc_vhm {

void utils::saveBinaryFile(float *vec, size_t len, const std::string &file) {
    std::ofstream out(file, std::ios::out | std::ios::binary);
    if (!out.is_open())
        return;
    out.write((const char *)vec, sizeof(float) * len);
    out.close();
}

std::vector<uint8_t> utils::readBinaryFile(const std::string &file) {
    std::ifstream in(file, std::ios::in | std::ios::binary);
    if (!in.is_open())
        return {};

    in.seekg(0, std::ios::end);
    size_t length = in.tellg();

    std::vector<uint8_t> data;
    if (length > 0) {
        in.seekg(0, std::ios::beg);
        data.resize(length);

        in.read((char *)&data[0], length);
    }
    in.close();
    return data;
}

std::vector<unsigned char> utils::loadModel(const std::string &file) {
    std::ifstream in(file, std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        return {};
    }
    in.seekg(0, std::ios::end);
    size_t length = in.tellg();

    std::vector<uint8_t> data;
    if (length > 0) {
        in.seekg(0, std::ios::beg);
        data.resize(length);
        in.read((char *)&data[0], length);
    }
    in.close();
    return data;
}

std::string utils::getSystemTimeStr() {
    return std::to_string(std::rand());
}

bool utils::setInputStream(const utils::InputStream &source, const std::string &imagePath, const std::string &videoPath, const int &cameraID,
                           cv::VideoCapture &capture, int &totalBatches, int &delayTime, InitParameter &param) {
    int total_frames = 0;
    std::string img_format;
    switch (source) {
    case utils::InputStream::IMAGE:
        img_format = imagePath.substr(imagePath.size() - 4, 4);
        if (img_format == ".png" || img_format == ".PNG") {
            sample::gLogWarning << "+-----------------------------------------------------------+" << std::endl;
            sample::gLogWarning << "| If you use PNG format pictures, the file name must be eg: |" << std::endl;
            sample::gLogWarning << "| demo0.png, demo1.png, demo2.png ......, but not demo.png. |" << std::endl;
            sample::gLogWarning << "| The above rules are determined by OpenCV.					|" << std::endl;
            sample::gLogWarning << "+-----------------------------------------------------------+" << std::endl;
        }
        capture.open(imagePath); // cv::CAP_IMAGES : !< OpenCV Image Sequence (e.g. img_%02d.jpg)
        param.batch_size = 1;
        total_frames = 1;
        totalBatches = 1;
        delayTime = 0;
        break;
    case utils::InputStream::VIDEO:
        capture.open(videoPath);
        total_frames = capture.get(cv::CAP_PROP_FRAME_COUNT);
        totalBatches = (total_frames % param.batch_size == 0) ? (total_frames / param.batch_size) : (total_frames / param.batch_size + 1);
        break;
    case utils::InputStream::CAMERA:
        // capture.open(cameraID);
        total_frames = INT_MAX;
        totalBatches = INT_MAX;
        break;
    default:
        break;
    }
    param.src_h = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    param.src_w = capture.get(cv::CAP_PROP_FRAME_WIDTH);

    return capture.isOpened();
}

void utils::setRenderWindow(InitParameter &param) {
    if (!param.is_show)
        return;
    int max_w = 960;
    int max_h = 540;
    float scale_h = (float)param.src_h / max_h;
    float scale_w = (float)param.src_w / max_w;
    if (scale_h > 1.f && scale_w > 1.f) {
        float scale = scale_h < scale_w ? scale_h : scale_w;
        cv::namedWindow(param.winname, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO); // for Linux
        cv::resizeWindow(param.winname, int(param.src_w / scale), int(param.src_h / scale));
        param.char_width = 16;
        param.det_info_render_width = 18;
        param.font_scale = 0.9;
    } else {
        cv::namedWindow(param.winname);
    }
}

std::string utils::getTimeStamp() {
    std::chrono::nanoseconds t = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return std::to_string(t.count());
}

void utils::show(const std::vector<std::vector<utils::Box>> &objectss, const std::vector<std::string> &classNames,
                 const int &cvDelayTime, std::vector<cv::Mat> &imgsBatch) {
    std::string windows_title = "image";
    if (!imgsBatch[0].empty()) {
        cv::namedWindow(windows_title, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO); // allow window resize(Linux)

        int max_w = 960;
        int max_h = 540;
        if (imgsBatch[0].rows > max_h || imgsBatch[0].cols > max_w) {
            cv::resizeWindow(windows_title, max_w, imgsBatch[0].rows * max_w / imgsBatch[0].cols);
        }
    }

    // vis
    cv::Scalar color = cv::Scalar(0, 255, 0);
    cv::Point bbox_points[1][4];
    const cv::Point *bbox_point0[1] = {bbox_points[0]};
    int num_points[] = {4};
    for (size_t bi = 0; bi < imgsBatch.size(); bi++) {
        if (!objectss.empty()) {
            for (auto &box : objectss[bi]) {
                if (classNames.size() == 91) // coco91
                {
                    color = Colors::color91[box.label];
                }
                if (classNames.size() == 80) // coco80
                {
                    color = Colors::color80[box.label];
                }
                if (classNames.size() == 20) // voc20
                {
                    color = Colors::color80[box.label];
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
        cv::imshow(windows_title, imgsBatch[bi]);
        cv::waitKey(cvDelayTime);
    }
}

void utils::save(const std::vector<std::vector<Box>> &objectss, const std::vector<std::string> &classNames,
                 const std::string &savePath, std::vector<cv::Mat> &imgsBatch, const int &batchSize, const int &batchi) {
    cv::Scalar color = cv::Scalar(0, 255, 0);
    cv::Point bbox_points[1][4];
    const cv::Point *bbox_point0[1] = {bbox_points[0]};
    int num_points[] = {4};
    for (size_t bi = 0; bi < imgsBatch.size(); bi++) {
        if (!objectss.empty()) {
            for (auto &box : objectss[bi]) {
                if (classNames.size() == 91) // coco91
                {
                    color = Colors::color91[box.label];
                }
                if (classNames.size() == 80) // coco80
                {
                    color = Colors::color80[box.label];
                }
                if (classNames.size() == 20) // voc20
                {
                    color = Colors::color20[box.label];
                }
                cv::rectangle(imgsBatch[bi], cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), color, 2, cv::LINE_AA);
                cv::String det_info = classNames[box.label] + " " + cv::format("%.4f", box.confidence);
                bbox_points[0][0] = cv::Point(box.left, box.top);
                bbox_points[0][1] = cv::Point(box.left + det_info.size() * 11, box.top);
                bbox_points[0][2] = cv::Point(box.left + det_info.size() * 11, box.top - 15);
                bbox_points[0][3] = cv::Point(box.left, box.top - 15);
                cv::fillPoly(imgsBatch[bi], bbox_point0, num_points, 1, color);
                cv::putText(imgsBatch[bi], det_info, bbox_points[0][0], cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

                if (!box.land_marks.empty()) {
                    for (auto &pt : box.land_marks) {
                        cv::circle(imgsBatch[bi], pt, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA, 0);
                    }
                }
            }
        }

        int imgi = batchi * batchSize + bi;
        cv::imwrite(savePath + "_" + std::to_string(imgi) + ".jpg", imgsBatch[bi]);
        cv::waitKey(1); // waitting for writting imgs
    }
}

utils::HostTimer::HostTimer() {
    t1 = std::chrono::steady_clock::now();
}

float utils::HostTimer::getUsedTime() {
    t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    return (1000 * time_used.count()); // ms
}

utils::HostTimer::~HostTimer() {
}

utils::DeviceTimer::DeviceTimer() {
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start);
}

float utils::DeviceTimer::getUsedTime() {
    cudaEventRecord(end);
    cudaEventSynchronize(end);
    float total_time;
    cudaEventElapsedTime(&total_time, start, end);
    return total_time;
}

utils::DeviceTimer::DeviceTimer(cudaStream_t stream) {
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start, stream);
}

float utils::DeviceTimer::getUsedTime(cudaStream_t stream) {
    cudaEventRecord(end, stream);
    cudaEventSynchronize(end);
    float total_time;
    cudaEventElapsedTime(&total_time, start, end);
    return total_time;
}

utils::DeviceTimer::~DeviceTimer() {
    cudaEventDestroy(start);
    cudaEventDestroy(end);
}

// utils::RtmpPusher::~RtmpPusher() {
//     if (fp_ != nullptr) {
//         fclose(fp_);
//     }
//     // // Write trailer and close output stream
//     // if (fmt_ctx != nullptr)
//     // {
//     // 	av_write_trailer(fmt_ctx);
//     // }

//     // // Destory Resources
//     // if (fmt_ctx != nullptr && fmt_ctx->pb != nullptr)
//     // {
//     // 	avio_close(fmt_ctx->pb);
//     // 	fmt_ctx->pb = nullptr;
//     // }
//     // if (fmt_ctx != nullptr)
//     // {
//     // 	avformat_free_context(fmt_ctx);
//     // }
//     // if (codec_ctx != nullptr)
//     // {
//     // 	avcodec_close(codec_ctx);
//     // }
//     // if (codec_ctx != nullptr)
//     // {
//     // 	avcodec_free_context(&codec_ctx);
//     // }
//     // fmt_ctx = nullptr;
//     // codec_ctx = nullptr;
// }

// int32_t utils::RtmpPusher::Init(std::string const &output_stream_url, cv::VideoCapture const &input_stream) {
//     std::stringstream command;
//     command << "ffmpeg ";

//     // infile options
//     command << "-y "               // overwrite output files
//             << "-an "              // close the audio output
//             << "-hide_banner "     // close the ffmpeg's version info
//             << "-f rawvideo "      // force format to rawvideo
//             << "-vcodec rawvideo " // force video rawvideo ('copy' to copy stream)
//             << "-pix_fmt bgr24 "   // set pixel format to bgr24
//             << "-s "               // set frame size (WxH or abbreviation)
//             << std::to_string(int(input_stream.get(3)))
//             << "x"
//             << std::to_string(int(input_stream.get(4)))
//             << " -r  " // set frame rate (Hz value, fraction or abbreviation)
//             << std::to_string(int(input_stream.get(5)));
//     command << " -i - "; //

//     // outfile options
//     command << "-vcodec libx264 "   // Hyper fast Audio and Video encoder
//             << " -pix_fmt yuv420p " // set pixel format to yuv420p
//             //<< "-preset ultrafast " // set the libx264 encoding preset to ultrafast
//             << "-f flv " // force format to flv
//                          //   << " -flvflags no_duration_filesize "
//             << output_stream_url;
//     rtmp_url_ = output_stream_url;
//     fp_ = popen(command.str().c_str(), "w");
//     if (fp_ == nullptr) {
//         return -1;
//     }
//     return 0;
//     // int32_t ret{0};
//     // // Initialize ffmpeg
//     // avcodec_register_all();
//     // av_register_all();
//     // avformat_network_init();
//     // // Open output stream
//     // avformat_alloc_output_context2(&fmt_ctx, nullptr, "flv", rtmp_url.c_str());
//     // if (fmt_ctx == nullptr)
//     // {
//     // 	std::cerr << "Failed to create output context!" << std::endl;
//     // 	return -1;
//     // }
//     // // Add video stream
//     // video_stream = avformat_new_stream(fmt_ctx, nullptr);
//     // if (video_stream == nullptr)
//     // {
//     // 	std::cerr << "Failed to create video stream!" << std::endl;
//     // 	return -1;
//     // }
//     // video_stream->codecpar->codec_id = AV_CODEC_ID_H264;
//     // video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
//     // video_stream->codecpar->width = stream_width;
//     // video_stream->codecpar->height = stream_height;
//     // video_stream->codecpar->format = AV_PIX_FMT_YUV420P;
//     // video_stream->codecpar->bit_rate = 200000;
//     // // Find video encoder
//     // codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//     // if (codec == nullptr)
//     // {
//     // 	std::cerr << "Failed to find video encoder!" << std::endl;
//     // 	return -1;
//     // }
//     // // Open video encoder
//     // codec_ctx = avcodec_alloc_context3(codec);
//     // if (codec_ctx == nullptr)
//     // {
//     // 	std::cerr << "Failed to allocate video encoder context!" << std::endl;
//     // 	return -1;
//     // }
//     // avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);
//     // codec_ctx->time_base = {1, static_cast<int>(stream_fps)};
//     // codec_ctx->thread_count = 4;

//     // ret = avcodec_open2(codec_ctx, codec, nullptr);
//     // if (ret < 0)
//     // {
//     // 	std::cerr << "Failed to open video encoder: " << ret << std::endl;
//     // 	return -1;
//     // }
//     // // Initialize output stream
//     // ret = avio_open(&fmt_ctx->pb, fmt_ctx->url, AVIO_FLAG_WRITE);
//     // if (ret < 0)
//     // {
//     // 	std::cerr << "Failed to open output stream: " << ret << std::endl;
//     // 	return -1;
//     // }
//     // // Write header
//     // ret = avformat_write_header(fmt_ctx, nullptr);
//     // if (ret < 0)
//     // {
//     // 	std::cerr << "Failed to write header: " << ret << std::endl;
//     // 	return -1;
//     // }
//     // return 0;
// }
// int32_t utils::RtmpPusher::PushOneFrame(cv::Mat const &frame) {
//     fwrite(frame.data, sizeof(char), frame.total() * frame.elemSize(), fp_);
//     return 0;
    // int32_t ret{0};
    // std::shared_ptr<AVFrame> picture_frame = ConvertToAVFrame(frame, AV_PIX_FMT_YUVA420P);
    // picture_frame->pts = pts++;
    // // 初始化AV PACKET
    // AVPacket *packet = av_packet_alloc();
    // av_init_packet(packet);
    // // 将生成出来的YUV帧发送至编码器
    // ret = avcodec_send_frame(codec_ctx, picture_frame.get());
    // if (ret < 0)
    // {
    // 	std::cerr << "Error: Could not send frame for encoding." << std::endl;
    // 	return -1;
    // }
    // // 获取转为H264的packet数据
    // while (true)
    // {
    // 	ret = avcodec_receive_packet(codec_ctx, packet);
    // 	if (ret < 0)
    // 	{
    // 		break;
    // 	}
    // 	ret = av_write_frame(fmt_ctx, packet);
    // 	if (ret < 0)
    // 	{
    // 		std::cerr << "Error: Could not write frame: " << ret << std::endl;
    // 		av_packet_unref(packet);
    // 		return -1;
    // 	}
    // }
    // av_packet_unref(packet);
    // return 0;
// }

// void utils::RtmpPusher::Handle(const std::vector<std::vector<utils::Box>> &objectss, const std::vector<std::string> &classNames,
//                                const int &cvDelayTime, std::vector<cv::Mat> &imgsBatch) {
//     std::string windows_title = "image";
//     if (!imgsBatch[0].empty()) {
//         cv::namedWindow(windows_title, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO); // allow window resize(Linux)

//         int max_w = 960;
//         int max_h = 540;
//         if (imgsBatch[0].rows > max_h || imgsBatch[0].cols > max_w) {
//             cv::resizeWindow(windows_title, max_w, imgsBatch[0].rows * max_w / imgsBatch[0].cols);
//         }
//     }

//     // vis
//     cv::Scalar color = cv::Scalar(0, 255, 0);
//     cv::Point bbox_points[1][4];
//     const cv::Point *bbox_point0[1] = {bbox_points[0]};
//     int num_points[] = {4};
//     for (size_t bi = 0; bi < imgsBatch.size(); bi++) {
//         if (!objectss.empty()) {
//             for (auto &box : objectss[bi]) {
//                 if (classNames.size() == 91) // coco91
//                 {
//                     color = Colors::color91[box.label];
//                 }
//                 if (classNames.size() == 80) // coco80
//                 {
//                     color = Colors::color80[box.label];
//                 }
//                 if (classNames.size() == 20) // voc20
//                 {
//                     color = Colors::color80[box.label];
//                 }
//                 cv::rectangle(imgsBatch[bi], cv::Point(box.left, box.top), cv::Point(box.right, box.bottom), color, 2, cv::LINE_AA);
//                 cv::String det_info = classNames[box.label] + " " + cv::format("%.4f", box.confidence);
//                 bbox_points[0][0] = cv::Point(box.left, box.top);
//                 bbox_points[0][1] = cv::Point(box.left + det_info.size() * 11, box.top);
//                 bbox_points[0][2] = cv::Point(box.left + det_info.size() * 11, box.top - 15);
//                 bbox_points[0][3] = cv::Point(box.left, box.top - 15);
//                 cv::fillPoly(imgsBatch[bi], bbox_point0, num_points, 1, color);
//                 cv::putText(imgsBatch[bi], det_info, bbox_points[0][0], cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

//                 if (!box.land_marks.empty()) // for facial landmarks
//                 {
//                     for (auto &pt : box.land_marks) {
//                         cv::circle(imgsBatch[bi], pt, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA, 0);
//                     }
//                 }
//             }
//         }
//         PushOneFrame(imgsBatch[bi]);
//         cv::waitKey(cvDelayTime);
//     }
// }

// std::string utils::RtmpPusher::GetUrl() const {
//     return rtmp_url_;
// }

// bool utils::RtmpPusher::ReadAVPixelFormatId(cv::Mat const &mat, AVPixelFormat &format)
// {
// 	int type = mat.type();
// 	uchar depth = type & CV_MAT_DEPTH_MASK;
// 	uchar channel = 1 + (type >> CV_CN_SHIFT);
// 	if (depth == CV_8U)
// 	{
// 		if (channel == 3)
// 		{
// 			format = AVPixelFormat::AV_PIX_FMT_BGR24;
// 			return true;
// 		}
// 		else if (channel == 4)
// 		{
// 			format = AVPixelFormat::AV_PIX_FMT_BGRA;
// 			return true;
// 		}
// 	}
// 	return false;
// }

// std::shared_ptr<AVFrame> utils::RtmpPusher::ConvertToAVFrame(cv::Mat const &mat, AVPixelFormat const &target_format)
// {
// 	AVPixelFormat src_format = AVPixelFormat::AV_PIX_FMT_NONE;
// 	if (!ReadAVPixelFormatId(mat, src_format))
// 	{
// 		std::cout << "read av pixel src_format id failed." << std::endl;
// 		return nullptr;
// 	}

// 	std::shared_ptr<AVFrame> frame(
// 		av_frame_alloc(), [](auto p)
// 		{ av_frame_free(&p); });

// 	frame->format = target_format;
// 	frame->width = mat.cols;
// 	frame->height = mat.rows;
// 	frame->pts = 0;
// 	int ret = av_frame_get_buffer(frame.get(), 32);
// 	if (ret < 0)
// 	{
// 		std::cout << "Could not allocate the video frame data." << std::endl;
// 		return nullptr;
// 	}

// 	ret = av_frame_make_writable(frame.get());
// 	if (ret < 0)
// 	{
// 		std::cout << "Av frame make writable failed." << std::endl;
// 		return nullptr;
// 	}

// 	SwsContext *swsctx = sws_getContext(
// 		frame->width,
// 		frame->height,
// 		src_format,
// 		frame->width,
// 		frame->height,
// 		target_format,
// 		SWS_BILINEAR,
// 		nullptr,
// 		nullptr,
// 		nullptr);

// 	const int stride[4] = {static_cast<int>(mat.step[0])};

// 	int scale_res = sws_scale(
// 		swsctx,
// 		&mat.data,
// 		stride,
// 		0,
// 		mat.rows,
// 		frame->data,
// 		frame->linesize);
// 	std::cout << "sws scale res=" << scale_res << std::endl;

// 	return frame;
// }

} // ns_uestc_vhm