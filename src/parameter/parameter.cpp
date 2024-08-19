#include "parameter.h"
#include "log.h"

namespace ns_uestc_vhm {
constexpr int32_t UESTC_VHM_VERSION_MAJOR = 0;
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
    // PRINT("\t[MUST] --model [recognition model path]: such as \"./yolov8n.trt\"\n");
    // PRINT("\t[OPT]  --batch [number of images processed in one round, default is 1]: such as \"1, 2 or 4...\"\n");
    PRINT("\t[MUST] --config [configuration file path]: such as \"./uestc_vhm_cfg.json\"\n");
    // PRINT("\t[OPT]  --save [recognition result saving directory]: such as \"./uestc_vhm_save_dir\"\n");
    // PRINT("\t[OPT]  --show: show the recognition result\n");
    PRINT("example: \n");
    // PRINT("\t./uestc_vhm --model=./yolov8n.trt --batch=4 --config=./uestc_vhm_cfg.json --save=./uestc_vhm_save_dir --show\n");
    PRINT("\t./uestc_vhm --config=./uestc_vhm_cfg.json\n");

    PRINT("-----------------------------------------------------------------------------------\n");
}

static void OptionVersion() {
    PRINT_INFO("app uestc_vhm version: v%d.%d.%d\n", UESTC_VHM_VERSION_MAJOR, UESTC_VHM_VERSION_MINOR, UESTC_VHM_VERSION_REVIS);
}

int32_t ParseOpt(int32_t argc, char **argv, ParamOpt &opt) {
    ShowCommand(argc, argv);
    cv::CommandLineParser opt_parser(argc, argv,
                                     {
                                         "{version || show uestc_vhm version   }"
                                         //   "{model   || tensorrt model file	    }"
                                         //   "{batch   || batch size               }"
                                         "{config  || configuration file path  }"
                                         //   "{save    || save path, can be ignore }"
                                         //   "{show    || if show the result       }"
                                     });
    if (!opt_parser.check()) {
        opt_parser.printErrors();
        OptionUsage();
        return -1;
    }

    if (opt_parser.has("version")) {
        OptionVersion();
        return 1;
    }

    // if (opt_parser.has("model")) {
    //     opt.model = opt_parser.get<std::string>("model");
    //     if (opt.model.empty()) {
    //         PRINT_ERROR("empty model path\n");
    //         OptionUsage();
    //         return -2;
    //     } else {
    //         PRINT_INFO("model path: %s\n", opt.model.c_str());
    //     }
    // }

    // if (opt_parser.has("batch")) {
    //     opt.param.batch_size = opt_parser.get<int32_t>("batch");
    //     std::string batch_str = opt_parser.get<std::string>("batch");
    //     if (batch_str.empty()) {
    //         PRINT_ERROR("empty batch size\n");
    //         OptionUsage();
    //         return -3;
    //     } else if (opt.param.batch_size == 0) {
    //         PRINT_ERROR("parse batch size failed, input: %s\n", batch_str.c_str());
    //         OptionUsage();
    //         return -4;
    //     } else {
    //         PRINT_INFO("batch size: %lu\n", opt.param.batch_size);
    //     }
    // }

    if (opt_parser.has("config")) {
        opt.config = opt_parser.get<std::string>("config");
        if (opt.config.empty()) {
            PRINT_ERROR("empty config path\n");
            OptionUsage();
            return -5;
        } else {
            PRINT_INFO("config path: %s\n", opt.config.c_str());
        }
    }

    // if (opt_parser.has("save")) {
    //     opt.param.save_path = opt_parser.get<std::string>("save");
    //     opt.param.is_save = true;
    //     if (opt.param.save_path.empty()) {
    //         PRINT_ERROR("empty save path\n");
    //         OptionUsage();
    //         return -6;
    //     } else {
    //         PRINT_INFO("save path: %s\n", opt.param.save_path.c_str());
    //     }
    // }

    // if (opt_parser.has("show")) {
    //     opt.param.is_show = true;
    //     PRINT_INFO("show results: true\n");
    // } else {
    //     PRINT_INFO("show results: false\n");
    // }

    // if (opt.model.empty()) {
    //     PRINT_ERROR("model path must be non-empty\n");
    //     OptionUsage();
    //     return -7;
    // }

    if (opt.config.empty()) {
        PRINT_ERROR("config path must be non-empty\n");
        OptionUsage();
        return -8;
    }

    return 0;
}

} // ns_uestc_vhm