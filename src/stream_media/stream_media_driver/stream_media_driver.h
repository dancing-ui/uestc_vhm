#ifndef _UESTC_VHM_STREAM_MEDIA_DRIVER_H_
#define _UESTC_VHM_STREAM_MEDIA_DRIVER_H_

#include "parameter.h"

namespace ns_uestc_vhm {
class StreamMediaDriver {
public:
    StreamMediaDriver() = default;
    virtual ~StreamMediaDriver() = default;

    StreamMediaDriver(StreamMediaDriver const &) = delete;
    StreamMediaDriver &operator=(StreamMediaDriver const &) = delete;
    StreamMediaDriver(StreamMediaDriver &&) = delete;
    StreamMediaDriver &operator=(StreamMediaDriver &&) = delete;

    virtual int32_t Init(StreamMediaCfgItem const &stream_media_cfg, ModelCfgItem const &model_cfg) = 0;
    virtual int32_t Start() = 0;
    virtual int32_t Stop() = 0;
    // virtual int32_t Handle();
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_STREAM_MEDIA_DRIVER_H_