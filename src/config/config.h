#ifndef _UESTC_VHM_CONFIG_H_
#define _UESTC_VHM_CONFIG_H_

#include <vector>

#include "parameter.h"

namespace ns_uestc_vhm {

class Config {
public:
    Config() = default;
    Config(ParamOpt const &opt);
    ~Config();

    Config(Config const &) = delete;
    Config &operator=(Config const &) = delete;
    Config(Config &&) = delete;
    Config &operator=(Config &&) = delete;

    int32_t Init();
    int32_t Parse();

    ParamOpt opt_;
    ModelCfgItem model_cfg_item_;
    std::vector<StreamMediaCfgItem> stream_media_cfg_items_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_CONFIG_H_