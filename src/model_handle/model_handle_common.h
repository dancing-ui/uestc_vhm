#ifndef _UESTC_VHM_MODEL_HANDLE_COMMON_H_
#define _UESTC_VHM_MODEL_HANDLE_COMMON_H_

#include <functional>

namespace ns_uestc_vhm {

using ModelHandleCb = std::function<int32_t(std::vector<std::vector<utils::Box>> const &,
                                            std::vector<std::string> const &,
                                            std::vector<cv::Mat> const &)>;

} // ns_uestc_vhm

#endif // _UESTC_VHM_MODEL_HANDLE_COMMON_H_