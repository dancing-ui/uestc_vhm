#include <csignal>
#include <cstdlib>
#include <thread>
#include <atomic>

#include "parameter.h"
#include "log.h"
#include "config.h"
#include "stream_media.h"

static std::atomic<bool> g_is_finished{false};

static void SignalHandler(int signum) {
    PRINT_INFO("catch signal=%d, exiting...\n", signum);
    g_is_finished.store(true);
}

int main(int argc, char **argv) {
    PRINT_INFO("begin app(uestc_vhm)\n");
    signal(SIGINT, SignalHandler);
    signal(SIGKILL, SignalHandler);
    ns_uestc_vhm::ParamOpt opt;
    int32_t ret{0};
    ret = ns_uestc_vhm::ParseOpt(argc, argv, opt);
    if (ret < 0) {
        PRINT_ERROR("parse args failed, ret=%d\n", ret);
        return -1;
    } else if (ret == 1) {
        return 0;
    }
    PRINT_INFO("parse args succeed\n");
    ns_uestc_vhm::Config cfg{opt};
    ret = cfg.Init();
    if (ret < 0) {
        PRINT_ERROR("init config failed, ret=%d\n", ret);
        return -1;
    }
    ret = cfg.Parse();
    if (ret < 0) {
        PRINT_ERROR("parse config failed, ret=%d\n", ret);
        return -1;
    }
    PRINT_INFO("parse config succeed\n");
    std::unique_ptr<ns_uestc_vhm::StreamMedia> stream_media = std::make_unique<ns_uestc_vhm::StreamMedia>();
    if (stream_media.get() == nullptr) {
        PRINT_ERROR("create stream_media failed\n");
        return -1;
    }
    ret = stream_media->Init(cfg);
    if (ret < 0) {
        PRINT_ERROR("init stream_media failed, ret=%d\n", ret);
        return -1;
    }
    PRINT_INFO("init stream_media succeed\n");
    ret = stream_media->Start();
    if (ret < 0) {
        PRINT_ERROR("start stream_media failed, ret=%d\n", ret);
        return -1;
    }
    PRINT_INFO("start stream_media succeed, main thread sleeping now\n");
    while (g_is_finished.load() == false) {
        sleep(1);
    }
    ret = stream_media->Stop();
    if (ret < 0) {
        PRINT_ERROR("stop stream_media failed, ret=%d\n", ret);
        return -1;
    }
    PRINT_INFO("stop stream_media succeed\n");
    PRINT_INFO("end app(uestc_vhm)\n");
    return 0;
}