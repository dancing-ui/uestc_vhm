#include "http_client.h"
#include "log.h"

#include <memory>

namespace ns_uestc_vhm {

int32_t HttpClient::Init() {
    http_client_ = std::make_unique<hv::HttpClient>();
    if (http_client_.get() == nullptr) {
        PRINT_ERROR("create http_client_ failed\n");
        return -1;
    }
    return 0;
}

int32_t HttpClient::SendRequest(HttpRequestPtr request) {
    if (request.get() == nullptr) {
        PRINT_ERROR("request is nullptr\n");
        return -1;
    }
    http_client_->sendAsync(request, [this](HttpResponsePtr response) -> void {
        HandleResponse(response);
    });
    return 0;
}

void HttpClient::HandleResponse(HttpResponsePtr response) {
    if (http_handle_cb_ && response.get()) {
        http_handle_cb_(response);
    }
}

} // ns_uestc_vhm