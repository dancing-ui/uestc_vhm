#include "reid_driver.h"

namespace ns_uestc_vhm {
namespace reid {

REID::REID(ModelCfgItem const &cfg) :
    cfg_(cfg) {
}

REID::~REID() {
}

int32_t REID::Init() {
    return 0;
}

int32_t REID::BatchInference(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes) {
    return 0;
}

std::vector<std::vector<cv::Mat>> REID::GetFeatLists() const {
    return feats_lists_;
}

} // reid

} // ns_uestc_vhm