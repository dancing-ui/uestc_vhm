#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <mutex>
#include <opencv2/core/hal/interface.h>
#include <cstdint>

#include "rocksdb.h"
#include "faiss_index.h"
#include "singleton.h"
#include "snowflake/snowflake.hpp"
#include "parameter.h"
#include "config.h"

namespace ns_uestc_vhm {
namespace dbase {

using SnowFlakeID = snowflake<1534832906275L, std::mutex>;

constexpr double distance_eps = 1e-8;

#define DELIMITER "_"

struct Recall {
    std::string cfid;
    float distance;
};

struct ClusterDict {
    /*
     * maintaining the following struct:
     *  - clusters dict: Map[cid] = [fid_1, fid_2, fid_3, ...]
     *  - cfids_to_ids dict: Map[cfid] = id
     *  - ids_to_cfids dict: Map[id] = cfid
     *
     * in which:
     *  1. cid: cluster id, tell which cluster the feature belong to
     *  2. fid: feature id, a unique id for the vector feature
     *  3. id: interal id for build faiss, because faiss require continue memory search
     *  4. cfid: cid + "_" + fid
     */

    std::unordered_map<std::string, std::vector<std::string>> clusters;
    std::unordered_map<std::string, int64_t> cfids_to_ids;
    std::unordered_map<int64_t, std::string> ids_to_cfids;

    void init() {
        this->_accumulator = 0;
    }

    void rehash(int64_t scale);
    void clear();
    void erase(std::string const &cid, std::string const &fid);
    void add(std::string const &cid, std::string const &fid);

    bool has(int64_t id);
    bool has(std::string const &cid);
    bool has(std::string const &cid, std::string const &fid);

    const std::vector<std::string> &query(std::string const &cid);
    const std::string &query(int64_t const &id);
    const int64_t &query(std::string const &cid, std::string const &fid);

    int64_t size(std::string &cid) {
        return this->clusters.at(cid).size();
    }

    int64_t size() {
        return this->clusters.size();
    }

    int64_t total_size() {
        return this->cfids_to_ids.size();
    }

    int64_t _accumulator = 0;
};

class DataBase {
public:
    void SetCfg(DataBaseParameter const &cfg);
    std::string GenerateFeatureID();

private:
    DataBaseParameter cfg_;
    SnowFlakeID gen_uuid_;

public:
    // unique name for the data base
    std::string db_name = "";
    int32_t db_version = 0;

    std::string db_create_time = "";
    std::string db_update_time = "";
    std::string db_delete_time = "";

    std::string search_device = "cpu";
    std::string feature_version = "";

    // 0: SMALL_SCALE, 1: MEDIUM_SCALE,
    // 2: LARGE_SCALE, 3: SUPER_SCALE, 4: EARTH_SCALE
    int32_t search_type = 0;

    // "L2": |x-y|^2, "cosine": 2 - 2 * dot(x,y)/|x||y|, "IP": dot(x,y)
    std::string similarity_type = "L2";

    int32_t feature_dim = 0;

    void create(
        std::string db_name,
        int32_t feature_dim,
        std::string search_device,
        std::string feature_version,
        int32_t search_type,
        std::string similarity_type);

    void clear();
    int32_t batch_add(std::vector<std::string> const &cids, std::vector<std::string> const &fids, std::vector<std::string> const &b64_features);
    int32_t batch_add_and_save(std::vector<std::string> const &cids, std::vector<std::string> const &fids, std::vector<std::string> const &b64_features);
    // void add(std::string &cid, std::string &fid, std::vector<float> &feature);
    // void remove(std::string &cid, std::string &fid);
    // void remove(std::string &cid);
    std::vector<std::vector<Recall>> batch_search(std::vector<std::string> const &b64_features, int32_t topk);
    int32_t restore(std::string &save_path);
    int64_t db_size() {
        return this->clusters.total_size();
    }

    int64_t db_cluster_size(std::string &cid) {
        return this->clusters.size(cid);
    }

    int64_t db_num_of_cluster() {
        return this->clusters.size();
    }

private:
    ClusterDict clusters;
    GpuFaissIndex faiss_index_;
    RocksDB save_db_;
    bool is_build = 0;
    bool is_l2_norm = 0;
    std::mutex g_mutex;
};

class DataBases {
    SINGLETON_DECL(DataBases)
public:
    DataBases() = default;
    ~DataBases() = default;

    void SetCfg(DataBaseParameter const &cfg);

public:
    std::unordered_map<std::string, DataBase *> dbs;

    DataBase *at(std::string const &db_name);
    bool has(std::string const &db_name);
    void create(
        std::string db_name,
        int32_t feature_dim,
        std::string search_device,
        std::string feature_version,
        int32_t search_type,
        std::string similarity_type);
    void restore(std::string db_name);
    void purge(std::string &db_name);

    int32_t num_of_dbs() {
        return this->dbs.size();
    }

private:
    std::mutex g_mutex;
    DataBaseParameter cfg_;
};

extern int32_t InitDB(Config const &cfg);

} // dbase
} // ns_uestc_vhm

#endif /* ifndef DATA_BASE_H */
