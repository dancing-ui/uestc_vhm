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

// #define LEVEL_1_SCALE 100000
// #define LEVEL_2_SCALE 1000000
// #define LEVEL_3_SCALE 10000000
// #define LEVEL_4_SCALE 100000000
// #define LEVEL_5_SCALE 1000000000

// #define LEVEL_1_NLIST 256   // 0.1M
// #define LEVEL_2_NLIST 512   // 1M
// #define LEVEL_3_NLIST 1024  // 10M
// #define LEVEL_4_NLIST 4096  // 0.1B
// #define LEVEL_5_NLIST 16384 // 1B

// #define LEVEL_1_NPROBE 2
// #define LEVEL_2_NPROBE 4
// #define LEVEL_3_NPROBE 8
// #define LEVEL_4_NPROBE 16
// #define LEVEL_5_NPROBE 32

// #define INIT_NLIST_SCALE 2
// #define NLIST LEVEL_3_NLIST
// #define NPROBE LEVEL_1_NPROBE
// #define EPSILON 0.05
// #define MIN_KMEAN_CLUSTER_SIZE 100 // min update sample in online kmean
// #define ALPHA 0.05                 // learning rate for online kmean

// typedef int64_t idx_t;

// /*
//  * a wrapper for faiss index
//  */
// class FaissIndex {
// public:
//     void init(uint32_t dim, std::string metric);
//     void search(std::vector<float> &x, int32_t topk, float *distance, int64_t *idx);
//     void erase(const int64_t &id);
//     void add_with_id(std::vector<float> &x, const int64_t &id);
//     void update_with_id(std::vector<float> &x, const int64_t &id);
//     void purge();

// private:
//     faiss::IndexFlat *_flat_index = nullptr;
//     faiss::IndexFlat *_quantizer = nullptr;
//     faiss::IndexIVFFlat *_index = nullptr;

//     faiss::IndexIDMap *index = nullptr;
//     faiss::IndexIDMap *flat_index = nullptr;

//     uint32_t n_centroids = NLIST;
//     uint32_t nprobe = NPROBE;
//     uint32_t init_n_centroids = 0;
//     uint32_t max_n = 0;
//     float alpha = ALPHA;

//     uint32_t feature_dim = 0;
//     faiss::MetricType metric_type;
//     std::string index_type;
//     std::string device;

//     std::vector<uint32_t> centroids_bucket_size;
//     std::vector<float> x_buff;
//     std::vector<int64_t> id_buff;
//     bool is_train_finish = false;
//     bool required_l2_norm = true;

//     void build_index();
//     void online_train_quantizer(const std::vector<float> &x, const int64_t &id);
//     void shrink_quantizer();
// };

class GpuFaissIndex {
public:
    int32_t init(uint32_t dim, std::string metric);
    int32_t batch_search(std::vector<float> const &query_feat, int64_t query_num, int32_t topk, float *distance, int64_t *idx);
    int32_t batch_erase(uint64_t del_num, std::vector<int64_t> const &ids);
    int32_t batch_add_with_ids(std::vector<float> const &add_feat, int64_t add_num, std::vector<int64_t> const &ids);
    int32_t batch_update_with_ids(std::vector<float> &update_feat, int64_t update_num, std::vector<int64_t> const &ids);
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
