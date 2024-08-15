#ifndef _UESTC_VHM_STREAM_MEDIA_H_
#define _UESTC_VHM_STREAM_MEDIA_H_

#include "config.h"
#include "stream_media_manager.h"
namespace ns_uestc_vhm {
class StreamMedia {
public:
    StreamMedia() = default;
    ~StreamMedia();

    StreamMedia(StreamMedia const &) = default;
    StreamMedia &operator=(StreamMedia const &) = default;

    int32_t Init(Config const &cfg);
    int32_t Start();
    int32_t Stop();
    // int32_t Handle();

private:
    std::unique_ptr<StreamMediaManager> stream_media_manager_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_STREAM_MEDIA_H_