#ifndef _UESTC_VHM_MODEL_FACTORY_H_
#define _UESTC_VHM_MODEL_FACTORY_H_

#include <memory>

#include "yolo_driver.h"
#include "reid_driver.h"

namespace ns_uestc_vhm {

class ModelFactory {
public:
    ModelFactory() = default;
    virtual ~ModelFactory() = default;

    ModelFactory(ModelFactory const &) = delete;
    ModelFactory &operator=(ModelFactory const &) = delete;
    ModelFactory(ModelFactory &&) = delete;
    ModelFactory &operator=(ModelFactory &&) = delete;

    virtual std::shared_ptr<yolo::YOLO> CreateYoloObj(ModelCfgItem const &cfg);
    virtual std::shared_ptr<reid::REID> CreateReidObj(ModelCfgItem const &cfg);
};

class YoloFactory : public ModelFactory {
public:
    YoloFactory() = default;
    ~YoloFactory() = default;

    YoloFactory(YoloFactory const &) = delete;
    YoloFactory &operator=(YoloFactory const &) = delete;
    YoloFactory(YoloFactory &&) = delete;
    YoloFactory &operator=(YoloFactory &&) = delete;

    std::shared_ptr<yolo::YOLO> CreateYoloObj(ModelCfgItem const &cfg) override;
};

class ReidFactory : public ModelFactory {
public:
    ReidFactory() = default;
    ~ReidFactory() = default;

    ReidFactory(ReidFactory const &) = delete;
    ReidFactory &operator=(ReidFactory const &) = delete;
    ReidFactory(ReidFactory &&) = delete;
    ReidFactory &operator=(ReidFactory &&) = delete;

    std::shared_ptr<reid::REID> CreateReidObj(ModelCfgItem const &cfg) override;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_FACTORY_H_