#ifndef _UESTC_VHM_CONFIG_H_
#define _UESTC_VHM_CONFIG_H_

#include <string>
#include <fstream>
#include <vector>

#include "parameter.h"

namespace ns_uestc_vhm {

struct StreamMediaCfgItem {
    int32_t id;
    bool is_enable;
    std::string type;
    std::string name;
    std::string in;
    std::string out;
    int32_t dst_width;
    int32_t dst_height;
};

struct ModelCfgItem {
    InitParameter param;
    std::string model;
};

class Config {
public:
    Config() = default;
    Config(ParamOpt const &opt);
    ~Config();

    Config(Config const &) = default;
    Config &operator=(Config const &) = default;

    int32_t Init();
    int32_t Parse();

    ParamOpt opt_;
    std::vector<StreamMediaCfgItem> stream_media_cfg_items_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_CONFIG_H_