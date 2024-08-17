#ifndef _UESTC_VHM_MODEL_FACTORY_H_
#define _UESTC_VHM_MODEL_FACTORY_H_

#include <memory>

#include "yolo_driver.h"

namespace ns_uestc_vhm {

class ModelFactory {
public:
    ModelFactory() = default;
    virtual ~ModelFactory() = default;

    ModelFactory(ModelFactory const &) = default;
    ModelFactory &operator=(ModelFactory const &) = default;
    ModelFactory(ModelFactory &&) = default;
    ModelFactory &operator=(ModelFactory &&) = default;

    virtual std::shared_ptr<yolo::YOLO> CreateYoloObj(ModelCfgItem const &cfg);
};

class YoloFactory : public ModelFactory {
public:
    YoloFactory() = default;
    ~YoloFactory() = default;

    YoloFactory(YoloFactory const &) = default;
    YoloFactory &operator=(YoloFactory const &) = default;
    YoloFactory(YoloFactory &&) = default;
    YoloFactory &operator=(YoloFactory &&) = default;

    std::shared_ptr<yolo::YOLO> CreateYoloObj(ModelCfgItem const &cfg) override;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_FACTORY_H_