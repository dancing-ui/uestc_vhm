#ifndef _UESTC_VHM_NETWORK_COMMON_H_
#define _UESTC_VHM_NETWORK_COMMON_H_

#include "hv/HttpMessage.h"
#include <string>
#include <cstdint>
#include <functional>
#include <hv/httpdef.h>

namespace ns_uestc_vhm {

enum class HttpMethod {
    kGet = 0,
    kHead = 1,
    kPost = 2,
    kPut = 3,
    kDelete = 4,
    kConnect = 5,
    kOptions = 6,
    kTrace = 7,
    kPatch = 8,
};

using HttpHandleCb = std::function<void(HttpResponsePtr)>;

} // ns_uestc_vhm

#endif // _UESTC_VHM_NETWORK_COMMON_H_