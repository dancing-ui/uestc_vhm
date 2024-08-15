#include "model_factory.h"
#include "yolov8.h"

namespace ns_uestc_vhm {

std::shared_ptr<yolo::YOLO> ModelFactory::CreateYoloObj(ModelCfgItem const &cfg) {
    return nullptr;
}

std::shared_ptr<yolo::YOLO> YoloFactory::CreateYoloObj(ModelCfgItem const &cfg) {
    return std::make_shared<YOLOV8>(cfg);
}
} // ns_uestc_vhm