#include <csignal>
#include <cstdlib>
#include <thread>
#include <atomic>

#include "parameter.h"
#include "log.h"
#include "config.h"
#include "stream_media.h"

static ns_uestc_vhm::StreamMedia g_stream_media;

static std::atomic<bool> is_finished{false};

static void SignalHandler(int signum) {
    PRINT_INFO("catch signal=%d, exiting...\n", signum);
    is_finished.store(true);
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
    ret = g_stream_media.Init(cfg);
    if (ret < 0) {
        PRINT_ERROR("init g_stream_media failed, ret=%d\n", ret);
        return -1;
    }
    ret = g_stream_media.Start();
    if (ret < 0) {
        PRINT_ERROR("start g_stream_media failed, ret=%d\n", ret);
        return -1;
    }
    while (is_finished.load() == false) {
        sleep(1);
    }
    ret = g_stream_media.Stop();
    if (ret < 0) {
        PRINT_ERROR("stop g_stream_media failed, ret=%d\n", ret);
        return -1;
    }
    PRINT_INFO("end app(uestc_vhm)\n");
    return 0;
}