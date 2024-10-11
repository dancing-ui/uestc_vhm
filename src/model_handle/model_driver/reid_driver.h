#ifndef _UESTC_VHM_REID_DRIVER_H_
#define _UESTC_VHM_REID_DRIVER_H_

#include "utils.h"

namespace ns_uestc_vhm {
namespace reid {

class REID {
public:
    REID(ModelCfgItem const &cfg);
    virtual ~REID();

    REID(REID const &) = delete;
    REID &operator=(REID const &) = delete;
    REID(REID &&) = delete;
    REID &operator=(REID &&) = delete;

    virtual int32_t Init();
    virtual int32_t BatchInference(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes);
    std::vector<std::vector<cv::Mat>> GetFeatLists() const;

protected:
    ModelCfgItem cfg_;
    std::vector<std::vector<cv::Mat>> feats_lists_;

private:
};

} // reid
} // ns_uestc_vhm

#endif // _UESTC_VHM_REID_DRIVER_H_