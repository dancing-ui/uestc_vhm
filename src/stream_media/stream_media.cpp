#include "stream_media.h"

namespace ns_uestc_vhm {

StreamMedia::~StreamMedia() {
}

int32_t StreamMedia::Init(Config const &cfg) {
    int32_t ret{0};
    stream_media_manager_ = std::make_unique<StreamMediaManager>();
    if (stream_media_manager_.get() == nullptr) {
        return -1;
    }
    ret = stream_media_manager_->Init(cfg);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int32_t StreamMedia::Start() {
    return stream_media_manager_->Start();
}

int32_t StreamMedia::Stop() {
    return stream_media_manager_->Stop();
}

} // ns_uestc_vhm