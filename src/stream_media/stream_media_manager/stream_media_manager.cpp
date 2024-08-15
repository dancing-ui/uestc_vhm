#include "stream_media_manager.h"

#include "log.h"

namespace ns_uestc_vhm {

StreamMediaManager::~StreamMediaManager() {
}

int32_t StreamMediaManager::Init(Config const &cfg) {
    int32_t ret{0};
    ModelCfgItem model_item{cfg.opt_.param, cfg.opt_.model};
    // parse rtmp config
    for (auto &&rtmp_item : cfg.stream_media_cfg_items_) {
        if (!rtmp_item.is_enable || rtmp_item.type != "rtmp") {
            continue;
        } 
        if (stream_media_drivers_.count(rtmp_item.id)) {
            PRINT_ERROR("repeated rtmp's id(%d)\n", rtmp_item.id);
            return -1;
        } else {
            stream_media_factory_ = std::make_shared<RtmpHandlerFactory>();
            if (stream_media_factory_.get() == nullptr) {
                PRINT_ERROR("create rtmp's stream_media_factory_ failed\n");
                return -1;
            }
            std::shared_ptr<StreamMediaDriver> stream_media_driver = std::move(stream_media_factory_->Create());
            if (stream_media_driver.get() == nullptr) {
                PRINT_ERROR("create rtmp's stream_media_driver failed\n");
                return -1;
            }
            ret = stream_media_driver->Init(rtmp_item, model_item);
            if (ret < 0) {
                PRINT_ERROR("init rtmp's stream_media_driver failed, ret=%d\n", ret);
                return -1;
            }
            stream_media_drivers_.insert({rtmp_item.id, stream_media_driver});
        }
    }
    return 0;
}

int32_t StreamMediaManager::Start() {
    // start rtmp
    for (auto iter = stream_media_drivers_.begin(); iter != stream_media_drivers_.end(); iter++) {
        PRINT_INFO("begin to start thread, stream_id=%d\n", iter->first);
        std::shared_ptr<std::thread> driver_th = std::make_shared<std::thread>(&StreamMediaDriver::Start, iter->second);
        if (driver_th.get() == nullptr) {
            PRINT_ERROR("start thread failed, stream_id=%d\n", iter->first);
            return -1;
        }
        PRINT_INFO("start thread success, stream_id=%d\n", iter->first);
        stream_media_ths_.insert({iter->first, driver_th});
    }
    return 0;
}

int32_t StreamMediaManager::Stop() {
    int32_t ret{0};
    for (auto iter = stream_media_drivers_.begin(); iter != stream_media_drivers_.end(); iter++) {
        PRINT_INFO("begin to stop driver, stream_id=%d\n", iter->first);
        int32_t temp_ret = iter->second->Stop();
        if (temp_ret < 0) {
            PRINT_ERROR("stop driver failed, stream_id=%d, ret=%d\n", iter->first, temp_ret);
            ret = -1;
        }
        PRINT_INFO("stop driver success, stream_id=%d\n", iter->first);
    }
    for (auto iter = stream_media_ths_.begin(); iter != stream_media_ths_.end(); iter++) {
        PRINT_INFO("begin to exit thread, stream_id=%d\n", iter->first);
        if (iter->second->joinable()) {
            iter->second->join();
        }
        PRINT_INFO("exit thread success, stream_id=%d\n", iter->first);
    }
    if(ret < 0) {
        return -1;
    }
    return 0;
}

} // ns_uestc_vhm