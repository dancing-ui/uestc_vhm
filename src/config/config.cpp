#include <unistd.h>
#include <vector>

#include "config.h"
#include "log.h"

#include "nlohmann/json.hpp"
#include "parameter.h"

namespace ns_uestc_vhm {

Config::Config(ParamOpt const &opt) :
    opt_(opt) {
}

Config::~Config() {
}

int32_t Config::Init() {
    if (0 != access(opt_.config.c_str(), F_OK)) {
        PRINT_ERROR("The file(%s) does not exist\n", opt_.config.c_str());
        return -1;
    }
    return 0;
}

int32_t Config::Parse() {
    nlohmann::json json_obj;
    std::ifstream ifs(opt_.config, std::ios::in);
    ifs >> json_obj;
    ifs.close();
    StreamMediaCfgItem stream_media_cfg_item;
    PRINT_INFO("begin to parse rtmp config\n");
    if (json_obj.contains("stream_media")) {
        for (const auto &item : json_obj["stream_media"]) {
            if (item.contains("in")) {
                stream_media_cfg_item.in = item["in"];
            } else {
                PRINT_ERROR("'in' key not found\n");
                return -1;
            }

            if (item.contains("out")) {
                stream_media_cfg_item.out = item["out"];
            } else {
                PRINT_ERROR("'out' key not found\n");
                return -1;
            }

            if (item.contains("dst_width")) {
                stream_media_cfg_item.dst_width = item["dst_width"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_width' key not found\n");
                return -1;
            }

            if (item.contains("dst_height")) {
                stream_media_cfg_item.dst_height = item["dst_height"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_height' key not found\n");
                return -1;
            }

            if (item.contains("name")) {
                stream_media_cfg_item.name = item["name"];
            } else {
                PRINT_ERROR("'name' key not found\n");
                return -1;
            }

            if (item.contains("id")) {
                stream_media_cfg_item.id = item["id"].get<int32_t>();
            } else {
                PRINT_ERROR("'id' key not found\n");
                return -1;
            }

            if (item.contains("enable")) {
                stream_media_cfg_item.is_enable = item["enable"].get<bool>();
            } else {
                PRINT_ERROR("'enable' key not found\n");
                return -1;
            }

            if (item.contains("type")) {
                stream_media_cfg_item.type = item["type"];
            } else {
                PRINT_ERROR("'type' key not found\n");
                return -1;
            }

            stream_media_cfg_items_.push_back(stream_media_cfg_item);
        }
    }
    PRINT_INFO("begin to parse model config\n");
    if (json_obj.contains("model")) {
        if (json_obj["model"].contains("yolo")) {
            auto item = json_obj["model"]["yolo"];

            if (item.contains("dst_width")) {
                model_cfg_item_.param.dst_w = item["dst_width"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_width' key not found\n");
                return -1;
            }

            if (item.contains("dst_height")) {
                model_cfg_item_.param.dst_h = item["dst_height"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_height' key not found\n");
                return -1;
            }

            if (item.contains("num_class")) {
                model_cfg_item_.param.num_class = item["num_class"].get<int32_t>();
            } else {
                PRINT_ERROR("'num_class' key not found\n");
                return -1;
            }

            if (item.contains("input_output_names")) {
                model_cfg_item_.param.input_output_names = item["input_output_names"].get<std::vector<std::string>>();
            } else {
                PRINT_ERROR("'input_output_names' key not found\n");
                return -1;
            }

            if (item.contains("conf_thresh")) {
                model_cfg_item_.param.conf_thresh = item["conf_thresh"].get<float>();
            } else {
                PRINT_ERROR("'conf_thresh' key not found\n");
                return -1;
            }

            if (item.contains("iou_thresh")) {
                model_cfg_item_.param.iou_thresh = item["iou_thresh"].get<float>();
            } else {
                PRINT_ERROR("'iou_thresh' key not found\n");
                return -1;
            }

            if (item.contains("path")) {
                model_cfg_item_.param.model_path = item["path"];
            } else {
                PRINT_ERROR("'path' key not found\n");
                return -1;
            }

            if (item.contains("batch")) {
                model_cfg_item_.param.batch_size = item["batch"].get<int32_t>();
            } else {
                PRINT_ERROR("'batch' key not found\n");
                return -1;
            }

            if (item.contains("save_dir")) {
                model_cfg_item_.param.save_path = item["save_dir"];
                model_cfg_item_.param.is_save = true;
            } else {
                PRINT_ERROR("'save_dir' key not found\n");
                return -1;
            }

            if (item.contains("show")) {
                model_cfg_item_.param.is_show = item["show"].get<bool>();
            } else {
                PRINT_ERROR("'show' key not found\n");
                return -1;
            }
        }
        if (json_obj["model"].contains("reid")) {
            auto item = json_obj["model"]["reid"];

            if (item.contains("engine_path")) {
                model_cfg_item_.reid_param.engine_path = item["engine_path"];
            } else {
                PRINT_ERROR("'engine_path' key not found\n");
                return -1;
            }

            if (item.contains("input_width")) {
                model_cfg_item_.reid_param.input_width = item["input_width"].get<int32_t>();
            } else {
                PRINT_ERROR("'input_width' key not found\n");
                return -1;
            }

            if (item.contains("input_height")) {
                model_cfg_item_.reid_param.input_height = item["input_height"].get<int32_t>();
            } else {
                PRINT_ERROR("'input_height' key not found\n");
                return -1;
            }

            if (item.contains("output_size")) {
                model_cfg_item_.reid_param.output_size = item["output_size"].get<int32_t>();
            } else {
                PRINT_ERROR("'output_size' key not found\n");
                return -1;
            }

            if (item.contains("device_id")) {
                model_cfg_item_.reid_param.device_id = item["device_id"].get<int32_t>();
            } else {
                PRINT_ERROR("'device_id' key not found\n");
                return -1;
            }

            if (item.contains("batch_size")) {
                model_cfg_item_.reid_param.batch_size = item["batch_size"].get<int32_t>();
            } else {
                PRINT_ERROR("'batch_size' key not found\n");
                return -1;
            }
        }
    }
    PRINT_INFO("begin to parse object track config\n");
    if (json_obj.contains("object_track")) {
        auto item = json_obj["object_track"];
        if (item.contains("enbaled_strategy_name")) {
            model_cfg_item_.object_track_param.enbaled_strategy_name = item["enbaled_strategy_name"].get<std::string>();
        } else {
            PRINT_ERROR("'enbaled_strategy_name' key not found\n");
            return -1;
        }

        if (json_obj["object_track"].contains("deepsort")) {
            auto item = json_obj["object_track"]["deepsort"];

            if (item.contains("max_age")) {
                model_cfg_item_.object_track_param.deepsort_param.max_age = item["max_age"].get<int32_t>();
            } else {
                PRINT_ERROR("'max_age' key not found\n");
                return -1;
            }

            if (item.contains("iou_threshold")) {
                model_cfg_item_.object_track_param.deepsort_param.iou_threshold = item["iou_threshold"].get<float>();
            } else {
                PRINT_ERROR("'iou_threshold' key not found\n");
                return -1;
            }

            if (item.contains("sim_threshold")) {
                model_cfg_item_.object_track_param.deepsort_param.sim_threshold = item["sim_threshold"].get<float>();
            } else {
                PRINT_ERROR("'sim_threshold' key not found\n");
                return -1;
            }

            if (item.contains("agnostic")) {
                model_cfg_item_.object_track_param.deepsort_param.agnostic = item["agnostic"].get<bool>();
            } else {
                PRINT_ERROR("'agnostic' key not found\n");
                return -1;
            }
        }
    }

    model_cfg_item_.work_mode = opt_.work_mode;
    return 0;
}

} // ns_uestc_vhm