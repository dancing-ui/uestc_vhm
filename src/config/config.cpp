#include <unistd.h>

#include "config.h"
#include "log.h"

#include "nlohmann/json.hpp"

namespace ns_uestc_vhm {

Config::Config(ParamOpt const &opt) :
    opt_(opt) {
}

Config::~Config() {
}

int32_t Config::Init() {
    if (0 != access(opt_.config.c_str(), F_OK)) {
        PRINT_ERROR("The file(%s) does not exist", opt_.config.c_str());
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
    if (json_obj.contains("stream_media")) {
        for (const auto &item : json_obj["stream_media"]) {
                        if (item.contains("in")) {
                stream_media_cfg_item.in = item["in"];
            } else {
                PRINT_ERROR("'in' key not found");
                return -1;
            }

            if (item.contains("out")) {
                stream_media_cfg_item.out = item["out"];
            } else {
                PRINT_ERROR("'out' key not found");
                return -2;
            }

            if (item.contains("dst_width")) {
                stream_media_cfg_item.dst_width = item["dst_width"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_width' key not found");
                return -3;
            }

            if (item.contains("dst_height")) {
                stream_media_cfg_item.dst_height = item["dst_height"].get<int32_t>();
            } else {
                PRINT_ERROR("'dst_height' key not found");
                return -4;
            }

            if (item.contains("name")) {
                stream_media_cfg_item.name = item["name"];
            } else {
                PRINT_ERROR("'name' key not found");
                return -5;
            }

            if (item.contains("id")) {
                stream_media_cfg_item.id = item["id"].get<int32_t>();
            } else {
                PRINT_ERROR("'id' key not found");
                return -6;
            }

            if (item.contains("enable")) {
                stream_media_cfg_item.is_enable = item["enable"].get<bool>();
            } else {
                PRINT_ERROR("'enable' key not found");
                return -7;
            }

            if (item.contains("type")) {
                stream_media_cfg_item.type = item["type"];
            } else {
                PRINT_ERROR("'type' key not found");
                return -8;
            }

            stream_media_cfg_items_.push_back(stream_media_cfg_item);
        }
    }
    return 0;
}

} // ns_uestc_vhm