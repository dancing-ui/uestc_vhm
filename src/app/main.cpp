#include <csignal>
#include <cstdlib>

#include "parameter.h"
#include "log.h"

static void SignalHandler(int signum) {
    PRINT_INFO("catch signal=%d, exiting...\n", signum);
    exit(0);
}

int main(int argc, char **argv) {
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
    while (1) {
        sleep(1);
    }
    return 0;
}