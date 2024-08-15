#ifndef _UESTC_VHM_STREAM_MEDIA_DRIVER_H_
#define _UESTC_VHM_STREAM_MEDIA_DRIVER_H_

#include "config.h"

namespace ns_uestc_vhm {
class StreamMediaDriver {
public:
    StreamMediaDriver() = default;
    virtual ~StreamMediaDriver() = default;

    StreamMediaDriver(StreamMediaDriver const &) = default;
    StreamMediaDriver &operator=(StreamMediaDriver const &) = default;

    virtual int32_t Init(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) = 0;
    virtual int32_t Start() = 0;
    virtual int32_t Stop() = 0;
    // virtual int32_t Handle();
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_STREAM_MEDIA_DRIVER_H_