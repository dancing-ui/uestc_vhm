#ifndef _UESTC_VHM_HTTP_CLIENT_H_
#define _UESTC_VHM_HTTP_CLIENT_H_

#include "network_common.h"

#include <hv/http_client.h>
#include <cstdint>
#include <memory>

namespace ns_uestc_vhm {

class HttpClient {
public:
    HttpClient() = default;
    ~HttpClient() = default;

    HttpClient(const HttpClient &) = delete;
    HttpClient &operator=(const HttpClient &) = delete;
    HttpClient(HttpClient &&) = delete;
    HttpClient &operator=(HttpClient &&) = delete;

    int32_t Init();

    int32_t SendRequest(HttpRequestPtr request);
    void HandleResponse(HttpResponsePtr response);

private:
    HttpHandleCb http_handle_cb_;
    std::unique_ptr<hv::HttpClient> http_client_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_HTTP_CLIENT_H_