#ifndef _FAISS_INDEX_H
#define _FAISS_INDEX_H

#include "faiss/gpu/GpuIndexIVFFlat.h"
#include <cstdint>

#include <faiss/IndexFlat.h>
#include <faiss/IndexIVFFlat.h>
#include <faiss/MetaIndexes.h>

#include <faiss/gpu/GpuIndexFlat.h>
#include <faiss/gpu/StandardGpuResources.h>
#include <faiss/gpu/GpuIndexIVFPQ.h>
#include <memory>

namespace ns_uestc_vhm {
namespace dbase {

class GpuFaissIndex {
public:
    int32_t init(uint32_t dim, std::string metric);
    int32_t batch_search_vector(std::vector<float> const &query_feat, int64_t query_num, int32_t topk, float *distance, int64_t *idx);
    int32_t batch_add_with_ids(std::vector<float> const &add_feat, int64_t add_num, std::vector<int64_t> const &ids);
    int32_t purge();

private:
    std::unique_ptr<faiss::gpu::StandardGpuResources> flat_resources_;
    std::unique_ptr<faiss::gpu::GpuIndexFlat> base_flat_index_;
    std::unique_ptr<faiss::IndexIDMap> flat_index_;

    uint32_t feature_dim_{0};
    faiss::MetricType metric_type_{faiss::METRIC_L2};

    int32_t build_index();
};

} // dbase
} // ns_uestc_vhm

#endif /* ifndef DATA_BASE_H */
