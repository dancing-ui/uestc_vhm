#include "model_factory.h"
#include "yolov8.h"
#include "fast_reid.h"

#include <memory>

namespace ns_uestc_vhm {

std::shared_ptr<yolo::YOLO> ModelFactory::CreateYoloObj(ModelCfgItem const &cfg) {
    return nullptr;
}

std::shared_ptr<reid::REID> ModelFactory::CreateReidObj(ModelCfgItem const &cfg) {
    return nullptr;
}

std::shared_ptr<yolo::YOLO> YoloFactory::CreateYoloObj(ModelCfgItem const &cfg) {
    return std::make_shared<YOLOV8>(cfg);
}

std::shared_ptr<reid::REID> ReidFactory::CreateReidObj(ModelCfgItem const &cfg) {
    return std::make_shared<FASTREID>(cfg);
}

} // ns_uestc_vhm