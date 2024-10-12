#ifndef _UESTC_VHM_FAST_REID_H_
#define _UESTC_VHM_FAST_REID_H_
#include "reid_driver.h"
#include "fastrt/baseline.h"
#include <cstdint>
#include <memory>

namespace ns_uestc_vhm {
class FASTREID : public reid::REID {
public:
    FASTREID(ModelCfgItem const &cfg);
    ~FASTREID();

    FASTREID(FASTREID const &) = delete;
    FASTREID &operator=(FASTREID const &) = delete;
    FASTREID(FASTREID &&) = delete;
    FASTREID &operator=(FASTREID &&) = delete;

    int32_t Init() override;
    int32_t BatchInference(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes) override;

private:
    std::unique_ptr<fastrt::Baseline> baseline_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_FAST_REID_H_