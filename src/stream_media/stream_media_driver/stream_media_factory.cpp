#include "stream_media_factory.h"
#include "rtmp_handler.h"

namespace ns_uestc_vhm {

std::shared_ptr<StreamMediaDriver> RtmpHandlerFactory::Create() {
    return std::make_shared<RtmpHandler>();
}
} // ns_uestc_vhm