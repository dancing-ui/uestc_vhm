#include "parameter.h"
#include "log.h"

namespace ns_uestc_vhm {
constexpr int32_t UESTC_VHM_VERSION_MAJOR = 1;
constexpr int32_t UESTC_VHM_VERSION_MINOR = 0;
constexpr int32_t UESTC_VHM_VERSION_REVIS = 0;

static void ShowCommand(int argc, char **argv) {
    std::stringstream ss;
    for (int i = 0; i < argc; ++i) {
        ss << argv[i] << " ";
    }
    PRINT_INFO("current command: %s\n", ss.str().c_str());
}

static void OptionUsage() {
    PRINT("-----------------------------------------------------------------------------------\n");
    PRINT("uestc_vhm help:\n");
    PRINT("\t[OPT] --version: show version\n");

    PRINT("-----------------------------------------------------------------------------------\n");
    PRINT("\t[MUST] --config=[configuration file path]: such as \"./uestc_vhm_cfg.json\"\n");
    PRINT("\t[MUST] --mode=[work mode]: supported mode [\"object_track\", \"person_reid\"]\n");
    PRINT("example: \n");
    PRINT("\t./uestc_vhm --config=/workspace/src/etc/uestc_vhm_cfg.json --mode=object_track\n");
    PRINT("-----------------------------------------------------------------------------------\n");
}

static void OptionVersion() {
    PRINT_INFO("uestc_vhm version: v%d.%d.%d\n", UESTC_VHM_VERSION_MAJOR, UESTC_VHM_VERSION_MINOR, UESTC_VHM_VERSION_REVIS);
}

int32_t ParseOpt(int32_t argc, char **argv, ParamOpt &opt) {
    ShowCommand(argc, argv);
    cv::CommandLineParser opt_parser(argc, argv,
                                     {"{version v || show uestc_vhm version   }"
                                      "{config c  || configuration file path  }"
                                      "{help h  || show uestc_vhm help}"
                                      "{mode m  || work mode}"
                                      "{usage u || show uestc_vhm usage}"});
    if (!opt_parser.check()) {
        opt_parser.printErrors();
        OptionUsage();
        return -1;
    }

    if (opt_parser.has("version")) {
        OptionVersion();
        return 1;
    }

    if (opt_parser.has("help")) {
        opt_parser.printMessage();
        return 1;
    }

    if (opt_parser.has("usage")) {
        OptionUsage();
        return 1;
    }

    if (opt_parser.has("config")) {
        opt.config = opt_parser.get<std::string>("config");
        if (opt.config.empty()) {
            PRINT_ERROR("empty config path\n");
            OptionUsage();
            return -1;
        } else {
            PRINT_INFO("config path: %s\n", opt.config.c_str());
        }
    }

    if (opt.config.empty()) {
        PRINT_ERROR("config path must be non-empty\n");
        OptionUsage();
        return -1;
    }

    if (opt_parser.has("mode")) {
        if (opt_parser.get<std::string>("mode") == "object_track") {
            opt.work_mode = WorkMode::kObjectTrack;
            PRINT_INFO("work mode: object track\n");
        } else if (opt_parser.get<std::string>("mode") == "person_reid") {
            opt.work_mode = WorkMode::kPersonReid;
            PRINT_INFO("work mode: person reid\n");
        } else {
            PRINT_ERROR("unsupported work mode: %s\n", opt_parser.get<std::string>("mode").c_str());
            OptionUsage();
            return -1;
        }
    }

    if (opt.work_mode == WorkMode::kNoneMode) {
        PRINT_ERROR("unsupported work mode: %s\n", opt_parser.get<std::string>("mode").c_str());
        OptionUsage();
        return -1;
    }

    return 0;
}

} // ns_uestc_vhm