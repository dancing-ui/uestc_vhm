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

// void FaissIndex::init(uint32_t dim, std::string metric) {
//     this->feature_dim = dim;

//     if (metric == "L2") {
//         this->index_type = "IndexIVFFlatL2";
//         this->metric_type = faiss::METRIC_L2;
//     }

//     this->build_index();
// }

// void FaissIndex::build_index() {
//     if (this->index_type == "IndexIVFFlatL2") {
//         this->_quantizer = new faiss::IndexFlatL2(this->feature_dim);
//         this->_flat_index = new faiss::IndexFlatL2(this->feature_dim);
//         this->_index = new faiss::IndexIVFFlat(
//             this->_quantizer, this->feature_dim,
//             this->n_centroids, faiss::METRIC_L2);

//         this->flat_index = new faiss::IndexIDMap(this->_flat_index);
//         this->index = new faiss::IndexIDMap(this->_index);
//         this->init_n_centroids = this->n_centroids * INIT_NLIST_SCALE;
//         this->max_n = this->init_n_centroids * MIN_KMEAN_CLUSTER_SIZE;
//     }

//     // init the _quantizer, in other word, the centroids
//     // init k cluster centers zi by inserting random vector
//     // auto &centroids = this->_quantizer->xb;
//     // for (uint32_t i = 0; i < this->init_n_centroids; ++i) {
//     //     std::vector<float> vec = linalg::gen_uniform_sp(this->feature_dim, 1.0);
//     //     centroids.insert(centroids.end(), vec.begin(), vec.end());
//     //     this->centroids_bucket_size.push_back(0);
//     //     this->_quantizer->ntotal += 1;
//     // }

//     this->is_train_finish = false;
// }

// /*
//  * online train quantizer apply the  sphere online kmean algorithm
//  * detail see the ref
//  * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.111.8125&rep=rep1&type=pdf
//  */
// void FaissIndex::online_train_quantizer(const std::vector<float> &x, const int64_t &id) {
//     // auto &centroids = this->_quantizer->xb; // TODO: update the interface

//     if (this->_quantizer->ntotal < this->init_n_centroids) {
//         this->_quantizer->ntotal += 1;
//         this->_quantizer->add(1, x.data());
//         // centroids.insert(centroids.end(), x.begin(), x.end());
//         this->centroids_bucket_size.push_back(1);
//     } else {
//         // determine the closest center zi to x
//         int64_t topk_id;
//         int64_t *I = new int64_t[1];
//         float *D = new float[1];
//         this->_quantizer->search(1, &x[0], 1, D, I);

//         // e.g. random() > 0.1 * (1 - t / max_t)
//         float decay = 1.0 - 1.0 * this->id_buff.size() / this->max_n;
//         if (drand48() > EPSILON * decay) {
//             topk_id = I[0];
//         } else {
//             topk_id = int64_t(drand48() / (1.0 / this->init_n_centroids));
//         }

//         this->centroids_bucket_size[topk_id] += 1;
//         // LOG(INFO) << "@@@: " << I[0] << "-" << D[0];

//         // get the closest centroids;
//         auto start = topk_id * this->feature_dim;
//         std::vector<float> topk_z;
//         topk_z.insert(
//             topk_z.end(),
//             this->_quantizer->get_xb() + start,
//             this->_quantizer->get_xb() + start + this->feature_dim);

//         // update centroids z
//         for (uint32_t i = 0; i < this->feature_dim; ++i) {
//             topk_z[i] += this->alpha * (x[i] - topk_z[i]);
//         }

//         // l2_normalize
//         if (this->required_l2_norm == true) linalg::l2_normalize(topk_z);

//         // assign to centroids
//         for (uint32_t i = 0; i < this->feature_dim; ++i) {
//             // centroids[start + i] = topk_z[i];
//             this->_quantizer->reconstruct();
//         }

//         delete[] I;
//         delete[] D;
//     }

//     // utils::pprint(this->centroids_bucket_size);
// }

// void FaissIndex::shrink_quantizer() {
//     auto &centroids = this->_quantizer->xb; // TODO: update the interface

//     std::vector<uint32_t> idx = linalg::argsort(this->centroids_bucket_size, true);

//     pprint(this->centroids_bucket_size);
//     pprint(idx);

//     std::vector<float> new_centroids;
//     for (auto &i : idx) {
//         auto start = centroids.begin() + i * this->feature_dim;
//         new_centroids.insert(new_centroids.end(), start, start + this->feature_dim);
//         if (new_centroids.size() == this->n_centroids * this->feature_dim) {
//             break;
//         }
//     }

//     centroids.clear();
//     centroids = new_centroids;
//     this->_quantizer->ntotal = this->n_centroids;

//     PRINT_INFO("shrink quantizer finishing...\n");
//     PRINT_INFO("ths final centroids number is: %ld\n", this->_quantizer->ntotal);
//     PRINT_INFO("ths final centroids vec size is: %lu\n", centroids.size());
// }

// void FaissIndex::add_with_id(std::vector<float> &x, const int64_t &id) {
//     if (this->required_l2_norm == true) linalg::l2_normalize(x);

//     if (this->is_train_finish == false) {
//         this->flat_index->add_with_ids(1, &x[0], &id);

//         this->online_train_quantizer(x, id);

//         this->x_buff.insert(this->x_buff.end(), x.begin(), x.end());
//         this->id_buff.push_back(id);

//         if (this->id_buff.size() == this->max_n) {
//             PRINT_INFO("exceed the threadhold: %u\n", this->max_n);
//             PRINT_INFO("switch to new index for large data !\n");

//             this->shrink_quantizer();

//             this->_quantizer->is_trained = true;
//             this->_index->is_trained = true;
//             this->is_train_finish = true;

//             // this->index->train(this->n_centroids, &this->x_buff[0]);
//             auto size = this->id_buff.size();
//             this->index->add_with_ids(size, &this->x_buff[0], &this->id_buff[0]);
//             this->x_buff.clear();
//             this->id_buff.clear();
//             this->flat_index->reset();
//         }
//         // LOG(INFO) << "@@@:" << id << " " << this->_quantizer->is_trained;
//         // LOG(INFO) << "@@@:" << id << " " << this->_index->nlist;
//         // LOG(INFO) << "@@@:" << id << " " << this->_quantizer->ntotal;
//     } else {
//         // LOG(INFO) << "###:" << id << " " << this->_index->is_trained;
//         // LOG(INFO) << "###:" << id << " " << this->_index->nlist;
//         // LOG(INFO) << "###:" << id << " " << this->_index->ntotal;
//         this->index->add_with_ids(1, &x[0], &id);
//     }
// }

// void FaissIndex::update_with_id(std::vector<float> &x, const int64_t &id) {
//     this->erase(id);
//     this->add_with_id(x, id);
// }

// void FaissIndex::erase(const int64_t &id) {
//     faiss::IDSelectorRange del_id(id, id + 1);

//     if (this->is_train_finish == false) {
//         this->flat_index->remove_ids(del_id);
//     } else {
//         this->index->remove_ids(del_id);
//     }
// }

// void FaissIndex::purge() {
//     delete this->flat_index;
//     this->flat_index = nullptr;

//     delete this->index;
//     this->index = nullptr;

//     delete this->_quantizer;
//     this->_quantizer = nullptr;

//     delete this->_flat_index;
//     this->_flat_index = nullptr;

//     delete this->_index;
//     this->_index = nullptr;

//     this->x_buff.clear();
//     this->id_buff.clear();
//     this->feature_dim = 0;
// }

// void FaissIndex::search(
//     std::vector<float> &x,
//     int32_t topk,
//     float *distance,
//     int64_t *idx) {
//     if (this->required_l2_norm == true) linalg::l2_normalize(x);

//     if (this->is_train_finish == false) {
//         this->flat_index->search(1, &x[0], topk, distance, idx);
//     } else {
//         this->_index->nprobe = this->nprobe;
//         this->index->search(1, &x[0], topk, distance, idx);
//     }
// }
/* GpuFaissIndex */
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
int32_t GpuFaissIndex::batch_search(std::vector<float> const &query_feat, int64_t query_num, int32_t topk, float *distance, int64_t *idx) {
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("flat_index_ is nullptr\n");
        return -1;
    }
    assert(query_num > 0);
    flat_index_->search(query_num, query_feat.data(), topk, distance, idx);
    return 0;
}
int32_t GpuFaissIndex::batch_erase(uint64_t del_num, std::vector<int64_t> const &ids) {
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("flat_index_ is nullptr\n");
        return -1;
    }
    if (del_num == 0) {
        return 0;
    }
    faiss::IDSelectorBatch del_ids{del_num, ids.data()};
    try {
        flat_index_->remove_ids(del_ids); // warning: GPU can not remove
    } catch (std::exception &e) {
        std::cout << "Standard exception: " << e.what() << std::endl;
    }

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
int32_t GpuFaissIndex::batch_update_with_ids(std::vector<float> &update_feat, int64_t update_num, std::vector<int64_t> const &ids) {
    int32_t ret{0};
    if (flat_index_.get() == nullptr) {
        PRINT_ERROR("flat_index_ is nullptr\n");
        return -1;
    }
    ret = batch_erase(update_num, ids);
    if (ret < 0) {
        PRINT_ERROR("GpuFaissIndex::batch_update_with_ids batch_erase failed, ret=%d\n", ret);
        return -2;
    }
    ret = batch_add_with_ids(update_feat, update_num, ids);
    if (ret < 0) {
        PRINT_ERROR("GpuFaissIndex::batch_update_with_ids batch_add_with_ids failed, ret=%d\n", ret);
        return -3;
    }
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
