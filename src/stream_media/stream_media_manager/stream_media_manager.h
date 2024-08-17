#ifndef _UESTC_VHM_STREAM_MEDIA_MANAGER_H_
#define _UESTC_VHM_STREAM_MEDIA_MANAGER_H_

#include <memory>
#include <map>
#include <thread>

#include "config.h"
#include "stream_media_factory.h"

namespace ns_uestc_vhm {
class StreamMediaManager {
public:
    StreamMediaManager() = default;
    ~StreamMediaManager();

    StreamMediaManager(StreamMediaManager const &) = delete;
    StreamMediaManager &operator=(StreamMediaManager const &) = delete;
    StreamMediaManager(StreamMediaManager &&) = delete;
    StreamMediaManager &operator=(StreamMediaManager &&) = delete;

    int32_t Init(Config const &cfg);
    int32_t Start();
    int32_t Stop();
    // int32_t Handle();

private:
    std::shared_ptr<StreamMediaFactory> stream_media_factory_;
    std::map<int32_t, std::shared_ptr<std::thread>> stream_media_ths_;
    std::map<int32_t, std::shared_ptr<StreamMediaDriver>> stream_media_drivers_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_STREAM_MEDIA_MANAGER_H_