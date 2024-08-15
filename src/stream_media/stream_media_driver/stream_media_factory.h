#ifndef _UESTC_VHM_STREAM_MEDIA_FACTORY_H_
#define _UESTC_VHM_STREAM_MEDIA_FACTORY_H_

#include <memory>

#include "stream_media_driver.h"

namespace ns_uestc_vhm {

class StreamMediaFactory {
public:
    StreamMediaFactory() = default;
    virtual ~StreamMediaFactory() = default;

    StreamMediaFactory(StreamMediaFactory const &) = default;
    StreamMediaFactory &operator=(StreamMediaFactory const &) = default;

    virtual std::shared_ptr<StreamMediaDriver> Create() = 0;
};

class RtmpHandlerFactory : public StreamMediaFactory {
public:
    RtmpHandlerFactory() = default;
    ~RtmpHandlerFactory() = default;

    RtmpHandlerFactory(RtmpHandlerFactory const &) = default;
    RtmpHandlerFactory &operator=(RtmpHandlerFactory const &) = default;

    std::shared_ptr<StreamMediaDriver> Create() override;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_STREAM_MEDIA_FACTORY_H_