#include "faiss_index.h"
#include "faiss/IndexIDMap.h"
#include "faiss/gpu/GpuIndexFlat.h"
#include "faiss/gpu/StandardGpuResources.h"
#include "faiss/impl/IDSelector.h"
#include "log.h"
#include "linalg.h"
#include "cstring.h"
#include <memory>

namespace ns_uestc_vhm {
namespace dbase {

int32_t GpuFaissIndex::init(uint32_t dim, std::string metric) {
    int ret{0};
    feature_dim_ = dim;
    if (metric == "L2") {
        metric_type_ = faiss::METRIC_L2;
    }
    ret = build_index();
    if (ret < 0) {
        PRINT_ERROR("GpuFaissIndex init failed, ret=%d\n", ret);
        return -1;
    }
    return ret;
}
int32_t GpuFaissIndex::batch_search_vector(std::vector<float> const &query_feat, int64_t query_num, int32_t topk, float *distance, int64_t *idx) {
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("flat_index_ is nullptr\n");
        return -1;
    }
    assert(query_num > 0);
    flat_index_->search(query_num, query_feat.data(), topk, distance, idx);
    return 0;
}

int32_t GpuFaissIndex::batch_add_with_ids(std::vector<float> const &add_feat, int64_t add_num, std::vector<int64_t> const &ids) {
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("flat_index_ is nullptr\n");
        return -1;
    }
    if (add_num == 0) {
        return 0;
    }
    flat_index_->add_with_ids(add_num, add_feat.data(), ids.data());
    return 0;
}

int32_t GpuFaissIndex::purge() {
    flat_resources_.reset();
    base_flat_index_->reset();
    flat_index_->reset();
    feature_dim_ = 0;
    return 0;
}

int32_t GpuFaissIndex::build_index() {
    flat_resources_ = std::make_unique<faiss::gpu::StandardGpuResources>();
    if (flat_resources_.get() == nullptr) {
        PRINT_ERROR("create flat_resources_ failed\n");
        return -1;
    }
    base_flat_index_ = std::make_unique<faiss::gpu::GpuIndexFlat>(flat_resources_.get(), feature_dim_, metric_type_);
    if (base_flat_index_.get() == nullptr) {
        PRINT_ERROR("create base_flat_index_ failed\n");
        return -2;
    }
    flat_index_ = std::make_unique<faiss::IndexIDMap>(base_flat_index_.get());
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("create flat_index_ failed\n");
        return -3;
    }
    return 0;
}

} // dbase
} // ns_uestc_vhm
