#include "database.h"
#include "log.h"
#include "base64.h"
#include "utils.h"
#include "cstring.h"

#include <cassert>
#include <cstddef>
#include <opencv2/core/hal/interface.h>

namespace ns_uestc_vhm {
namespace dbase {

void ClusterDict::rehash(int64_t scale) {
    this->cfids_to_ids.rehash(scale);
    this->ids_to_cfids.rehash(scale);
    this->clusters.rehash(scale);
}

void ClusterDict::clear() {
    this->clusters.clear();
    this->cfids_to_ids.clear();
    this->ids_to_cfids.clear();
}

const std::vector<std::string> &ClusterDict::query(std::string const &cid) {
    if (this->has(cid)) {
        return this->clusters.at(cid);
    } else {
        ThrowError("[ClusterDict::query] no such cid in cluster dict !");
    }
}

const std::string &ClusterDict::query(int64_t const &id) {
    if (this->has(id)) {
        return this->ids_to_cfids.at(id);
    } else {
        ThrowError("[ClusterDict::query] no such id in cluster dict !");
    }
}

const int64_t &ClusterDict::query(std::string const &cid, std::string const &fid) {
    if (this->has(cid, fid)) {
        std::string cfid = cid + DELIMITER + fid;
        return this->cfids_to_ids.at(cfid);
    } else {
        ThrowError("[ClusterDict::query] no such cid + fid in cluster dict !");
    }
}

bool ClusterDict::has(std::string const &cid, std::string const &fid) {
    std::string cfid = cid + DELIMITER + fid;
    auto it = this->cfids_to_ids.find(cfid);
    if (it != this->cfids_to_ids.end()) {
        return 1;
    } else {
        return 0;
    }
}

bool ClusterDict::has(int64_t id) {
    auto it = this->ids_to_cfids.find(id);
    if (it != this->ids_to_cfids.end()) {
        return 1;
    } else {
        return 0;
    }
}

bool ClusterDict::has(std::string const &cid) {
    auto it = this->clusters.find(cid);
    if (it != this->clusters.end()) {
        return 1;
    } else {
        return 0;
    }
}

void ClusterDict::erase(std::string const &cid, std::string const &fid) {
    std::string cfid = cid + DELIMITER + fid;
    int64_t id = this->cfids_to_ids.at(cfid);
    this->ids_to_cfids.erase(id);
    this->cfids_to_ids.erase(cfid);

    std::vector<std::string> &fids = this->clusters.at(cid);
    auto it = std::find(fids.begin(), fids.end(), fid);
    if (it != fids.end())
        fids.erase(it);

    // if cid become empty, delete this cid
    if (fids.size() == 0)
        this->clusters.erase(cid);
}

void ClusterDict::add(std::string const &cid, std::string const &fid, int64_t person_id_accumulator) {
    std::string cfid = cid + DELIMITER + fid;
    int64_t id = person_id_accumulator;
    this->cfids_to_ids.insert(std::make_pair(cfid, id));
    this->ids_to_cfids.insert(std::make_pair(id, cfid));

    if (this->clusters.find(cid) == this->clusters.end()) {
        std::vector<std::string> fid_list;
        this->clusters.insert(std::make_pair(cid, fid_list));
    }

    this->clusters.at(cid).push_back(fid);
}

void DataBase::SetCfg(DataBaseParameter const &cfg) {
    cfg_ = cfg;
}

std::string DataBase::GenerateFeatureID() {
    int64_t uuid = gen_uuid_.nextid();
    return utils::Int64ToStr(uuid);
}

void DataBase::create(
    std::string const &db_name,
    int32_t feature_dim,
    std::string const &search_device = "gpu",
    std::string const &feature_version = "v1",
    int32_t search_type = 0,
    std::string const &similarity_type = "L2") {
    // =====================

    if (this->is_build == 1)
        ThrowError("db_name: " + this->db_name + " have been created");

    this->db_name = db_name;
    this->feature_dim = feature_dim;
    this->db_create_time = utils::LocalTime();
    this->db_delete_time = "";
    this->search_device = search_device;
    this->feature_version = feature_version;
    this->search_type = search_type;
    this->similarity_type = similarity_type;

    this->clusters.init();
    save_db_.open(cfg_.save_path + "/" + this->db_name);
    save_db_.put("meta:db_name", this->db_name);
    save_db_.put("meta:feature_dim", this->feature_dim);
    save_db_.put("meta:search_device", this->search_device);
    save_db_.put("meta:feature_version", this->feature_version);
    save_db_.put("meta:search_type", this->search_type);
    save_db_.put("meta:similarity_type", this->similarity_type);
    save_db_.put("person_id_accumulator", 0);

    this->faiss_index_.init(feature_dim, similarity_type);

    this->is_build = 1;
    PRINT_INFO("[Create Database] name=%s,feature_dimension=%d,search_device=%s,feature_version=%s,search_type=%d,similarity_type=%s\n",
               this->db_name.c_str(), this->feature_dim, this->search_device.c_str(), this->feature_version.c_str(), this->search_type, this->similarity_type.c_str());
}

void DataBase::clear() {
    std::lock_guard<std::mutex> lock(this->g_mutex);
    if (this->is_build == 1) {
        this->faiss_index_.purge();
        this->db_name = "";
        this->feature_dim = 0;
        this->db_delete_time = utils::LocalTime();
        this->clusters.clear();
        save_db_.purge();
        this->is_build = 0;
        PRINT_INFO("success clear the data base !!!");
    } else {
        ThrowError("db_name: " + this->db_name + " do not exist");
    }
}

int32_t DataBase::batch_add_vector(std::vector<std::string> const &cids, std::vector<std::string> const &fids, std::vector<std::string> const &b64_features) {
    std::lock_guard<std::mutex> lock(this->g_mutex);
    ThrowErrorIf(this->is_build == 0, "should create a data base first");
    int32_t ret{0};
    assert(cids.size() == fids.size());
    assert(cids.size() == b64_features.size());
    std::vector<float> add_features;
    std::vector<int64_t> add_ids;
    for (size_t i = 0; i < cids.size(); i++) {
        std::vector<float> feature;
        utils::Base64Decode(b64_features[i], feature);

        if (!this->clusters.has(cids[i], fids[i])) {
            // cid and fid no exist, add a new one
            int64_t person_id_accumulator{-1};
            search("person_id_accumulator", person_id_accumulator);
            assert(person_id_accumulator != -1);
            this->clusters.add(cids[i], fids[i], person_id_accumulator);
            int64_t id = this->clusters.query(cids[i], fids[i]);
            add_features.insert(add_features.end(), feature.begin(), feature.end());
            add_ids.emplace_back(id);
            update("person_id_accumulator", ++person_id_accumulator);
        } else {
            // if cid and fid exist, nothings to do
        }
    }
    ret = this->faiss_index_.batch_add_with_ids(add_features, add_ids.size(), add_ids);
    if (ret < 0) {
        PRINT_ERROR("DataBase::batch_add_vector batch_add_with_ids failed, ret=%d\n", ret);
        return -1;
    }
    return 0;
}

int32_t DataBase::batch_add_vector_and_save(std::vector<std::string> const &cids, std::vector<std::string> const &fids, std::vector<std::string> const &b64_features) {
    assert(cids.size() == fids.size());
    assert(cids.size() == b64_features.size());
    int32_t ret{0};
    ret = batch_add_vector(cids, fids, b64_features);
    if (ret < 0) {
        PRINT_ERROR("DataBase::batch_add_vector_and_save batch_add_vector failed, ret=%d\n", ret);
        return -1;
    }
    for (size_t i = 0; i < b64_features.size(); i++) {
        std::string key = cids[i] + DELIMITER + fids[i];
        put(key, b64_features[i]); // TODO: batch put
    }
    return 0;
}

std::vector<std::vector<Recall>> DataBase::batch_search_vector(std::vector<std::string> const &b64_features, int32_t topk) {
    std::lock_guard<std::mutex> lock(this->g_mutex);
    ThrowErrorIf(this->is_build == 0, "should create a data base first");
    std::vector<float> features;
    int64_t query_num{static_cast<int64_t>(b64_features.size())};
    for (int64_t i = 0; i < query_num; i++) {
        std::vector<float> feature;
        utils::Base64Decode(b64_features[i], feature);
        features.insert(features.end(), feature.begin(), feature.end());
    }

    int64_t *recall_idx = new int64_t[query_num * topk];
    float *recall_distance = new float[query_num * topk];

    this->faiss_index_.batch_search_vector(features, query_num, topk, recall_distance, recall_idx);

    std::vector<std::vector<Recall>> all_recalls;
    for (int64_t i = 0; i < query_num; i++) {
        std::vector<Recall> recalls;
        for (int32_t j = 0; j < topk; j++) {
            Recall recall;
            int64_t id = recall_idx[i * topk + j];
            if (id > 0) {
                const std::string &cfid = this->clusters.query(id);
                if (recall_distance[i * topk + j] < cfg_.dist_thresh) {
                    recall.cfid = cfid;
                    recall.distance = recall_distance[i * topk + j];
                    recalls.emplace_back(recall);
                }
            }
        }
        all_recalls.emplace_back(recalls);
    }

    delete[] recall_idx;
    delete[] recall_distance;
    return all_recalls;
}

int32_t DataBase::restore(std::string const &db_name) {
    std::lock_guard<std::mutex> lock(this->g_mutex);
    int32_t ret{0};
    std::string save_path = cfg_.save_path + "/" + db_name;
    save_db_.open(save_path);
    save_db_.get("meta:db_name", this->db_name);
    save_db_.get("meta:feature_dim", this->feature_dim);
    save_db_.get("meta:search_device", this->search_device);
    save_db_.get("meta:feature_version", this->feature_version);
    save_db_.get("meta:search_type", this->search_type);
    save_db_.get("meta:similarity_type", this->similarity_type);

    std::vector<std::string> cids, fids, b64_features;

    auto it = save_db_.iter_key();
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        // only process non meta key
        if (key.find("meta:") == std::string::npos && key != "person_id_accumulator") {
            std::string b64_feature;
            save_db_.get(key, b64_feature);
            std::vector<std::string> cfid;
            utils::SplitString(key, cfid, DELIMITER);
            cids.emplace_back(cfid[0]);
            fids.emplace_back(cfid[1]);
            b64_features.emplace_back(b64_feature);
        }
    }
    ret = batch_add_vector(cids, fids, b64_features);
    if (ret < 0) {
        PRINT_ERROR("DataBase::restore batch_add_vector failed, ret=%d\n", ret);
        return -1;
    }

    PRINT_INFO("[Restore Database] name=%s,feature_dimension=%d,search_device=%s,feature_version=%s,search_type=%d,similarity_type=%s\n",
               this->db_name.c_str(), this->feature_dim, this->search_device.c_str(), this->feature_version.c_str(), this->search_type, this->similarity_type.c_str());
    return 0;
}

void DataBase::put(std::string const &key, std::string const &value) {
    save_db_.put(key, value);
}

void DataBase::put(std::string const &key, int64_t value) {
    save_db_.put(key, value);
}

void DataBase::search(std::string const &key, int64_t &value) {
    save_db_.get(key, value);
}

void DataBase::erase(std::string const &key) {
    save_db_.erase(key);
}

void DataBase::update(std::string const &key, int64_t value) {
    erase(key);
    put(key, value);
}

int64_t DataBase::query(std::string const &cid, std::string const &fid) {
    // TODO: clusters must be as a LRU Cache(not at all in memory, because the memory is finite) for person id,
    // or use rocksdb store the cfid==>id mapping
    if (clusters.has(cid, fid) == true) {
        return clusters.query(cid, fid);
    }
    return -1;
}

DataBase *DataBases::at(std::string const &db_name) {
    std::lock_guard<std::mutex> lock(this->g_mutex);

    if (this->has(db_name)) {
        auto *db = this->dbs.at(db_name);
        return db;
    } else {
        ThrowError("[DataBase &DataBases::at] db is no exist: " + db_name);
    }
}

bool DataBases::has(std::string const &db_name) {
    auto it = this->dbs.find(db_name);
    if (it != this->dbs.end())
        return 1;
    else
        return 0;
}

void DataBases::create(
    std::string const &db_name,
    int32_t feature_dim,
    std::string const &search_device = "gpu",
    std::string const &feature_version = "v1",
    int32_t search_type = 0,
    std::string const &similarity_type = "L2") {
    ///=====================
    std::lock_guard<std::mutex> lock(this->g_mutex);

    if (!this->has(db_name)) {
        DataBase *db = new DataBase;
        db->SetCfg(cfg_);
        db->create(
            db_name,
            feature_dim,
            search_device,
            feature_version,
            search_type,
            similarity_type);
        this->dbs.insert(std::make_pair(db_name, db));
    } else {
        ThrowError("[void DataBases::create] db is exist: " + db_name);
    }
}

void DataBases::restore(std::string const &db_name) {
    if (!this->has(db_name)) {
        DataBase *db = new DataBase;
        db->SetCfg(cfg_);
        db->restore(db_name);
        this->dbs.insert(std::make_pair(db_name, db));
    } else {
        ThrowError("[void DataBases::restore] db is exist: " + db_name);
    }
}

void DataBases::purge(std::string const &db_name) {
    std::lock_guard<std::mutex> lock(this->g_mutex);
    if (this->has(db_name)) {
        auto &db = this->dbs.at(db_name);
        db->clear();
        delete db;
        db = nullptr;
        this->dbs.erase(db_name);
    } else {
        ThrowError("[void DataBases::purge] db is no exist: " + db_name);
    }
}

void DataBases::SetCfg(DataBaseParameter const &cfg) {
    cfg_ = cfg;
}

SINGLETON_IMPL(DataBases)

int32_t InitDB(Config const &cfg) {
    auto database_cfg = cfg.model_cfg_item_.person_reid_param.database_param_;
    if (database_cfg.is_enable) {
        DataBases::instance()->SetCfg(database_cfg);
        switch (database_cfg.op_type) {
        case DBOperateType::kCreate: {
            DataBases::instance()->create(database_cfg.db_name, database_cfg.feature_dim);
            break;
        }
        case DBOperateType::kRestore: {
            DataBases::instance()->restore(database_cfg.db_name);
            break;
        }
        case DBOperateType::kNone: {
            PRINT_ERROR("none database operations, supported operations:[kCreate, kRestore]\n");
            return -1;
        }
        }
    }
    // here add more database init
    return 0;
}

} // dbase
} // ns_uestc_vhm
