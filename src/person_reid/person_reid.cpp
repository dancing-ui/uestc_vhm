#include "person_reid.h"
#include "base64.h"
#include "cstring.h"
#include "database.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <unordered_set>
#include <vector>
#include "database.h"
#include "model_handle_common.h"
#include "utils.h"
#include "log.h"

namespace ns_uestc_vhm {

int32_t PersonReidCtx::Init(PersonReidParameter const &cfg) {
    cfg_ = cfg;
    database_ = dbase::DataBases::instance()->at(cfg_.database_param_.db_name);
    if (database_ == nullptr) {
        PRINT_ERROR("PersonReidCtx::Init database_ is nullptr\n");
        return -1;
    }
    return 0;
}

int32_t PersonReidCtx::Reid(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists) {
    int32_t ret{0};
    assert(detect_boxes.size() == feats_lists.size());
    assert(imgs_batch.size() == feats_lists.size());

    reid_boxes_lists_.clear();
    for (size_t i = 0; i < feats_lists.size(); i++) {
        // init feature
        std::vector<TrackerRes> reid_boxes;
        std::vector<std::string> b64_features;
        for (size_t j = 0; j < feats_lists[i].size(); j++) {
            cv::Mat feat = feats_lists[i][j].clone();
            std::string b64_feat{convertMat2B64(feat)};
            b64_features.emplace_back(b64_feat);
        }
        // search topK
        std::vector<std::vector<dbase::Recall>> all_recalls{database_->batch_search_vector(b64_features, cfg_.top_k)};
        // add new feature
        std::vector<std::string> cids, fids;
        std::vector<std::string> add_b64_features;
        for (size_t j = 0; j < feats_lists[i].size(); j++) {
            if (all_recalls[j].empty()) {
                std::string fid{database_->GenerateFeatureID()};
                cids.push_back(cid_);
                fids.push_back(fid);
                add_b64_features.push_back(b64_features[j]);
            }
        }
        ret = database_->batch_add_vector_and_save(cids, fids, add_b64_features);
        if (ret < 0) {
            PRINT_ERROR("PersonReidCtx::Reid batch_add_vector_and_save failed, ret=%d\n", ret);
            return -1;
        }
        // confirm person id after add
        for (size_t j = 0, k = 0; j < feats_lists[i].size(); j++) {
            int64_t person_id{-1};
            if (all_recalls[j].empty()) {
                std::string cid{cid_};
                std::string fid{fids[k]};
                k++;
                person_id = database_->query(cid, fid);
            } else {
                person_id = confirmPersonId(all_recalls[j]);
            }
            assert(person_id != -1);
            TrackerRes reid_res{detect_boxes[i][j].label, detect_boxes[i][j].confidence,
                                detect_boxes[i][j].left, detect_boxes[i][j].top,
                                detect_boxes[i][j].width, detect_boxes[i][j].height,
                                person_id};
            reid_boxes.emplace_back(reid_res);
        }
        reid_boxes_lists_.emplace_back(reid_boxes);
    }
    return 0;
}

std::string PersonReidCtx::convertMat2B64(cv::Mat const &mat) {
    auto float_feat = utils::convertMat2Vector<float>(mat);
    std::string b64_mat;
    utils::Base64Encode(float_feat, b64_mat);
    return b64_mat;
}

std::vector<std::vector<TrackerRes>> PersonReidCtx::GetReidBoxesLists() const {
    return reid_boxes_lists_;
}

int64_t PersonReidCtx::confirmPersonId(std::vector<dbase::Recall> const &recall) {
    // get person id
    std::vector<std::pair<int64_t, float>> persons;
    std::unordered_set<int64_t> appeared_id;
    std::unordered_map<int64_t, int64_t> id_times;
    std::vector<std::string> cfid;

    utils::SplitString(recall[0].cfid, cfid, DELIMITER);
    int64_t person_id{0};
    person_id = database_->query(cfid[0], cfid[1]);
    assert(person_id != -1);
    persons.push_back({person_id, recall[0].distance});
    appeared_id.insert(person_id);
    id_times[person_id]++;

    if (recall.size() == 1) {
        return person_id;
    }

    bool is_equal_dist{true};
    std::pair<float, int64_t> min_dist{recall[0].distance, person_id};

    for (size_t i = 1; i < recall.size(); i++) {
        cfid.clear();
        utils::SplitString(recall[i].cfid, cfid, DELIMITER);
        person_id = database_->query(cfid[0], cfid[1]);
        assert(person_id != -1);
        persons.push_back({person_id, recall[i].distance});
        appeared_id.insert(person_id);
        id_times[person_id]++;
        if (min_dist.first > recall[i].distance) {
            min_dist = {recall[i].distance, person_id};
        }
        if (!utils::CheckFloatEqual(recall[i].distance, recall[0].distance)) {
            is_equal_dist = false;
        }
    }
    if (recall.size() == 1) {
        return persons[0].first;
    }
    // confirm person id
    if (appeared_id.size() == recall.size()) {
        if (is_equal_dist == true) {
            std::sort(persons.begin(), persons.end(), [](std::pair<int64_t, float> const &x, std::pair<int64_t, float> const &y) -> bool {
                return x.first > y.first;
            });
            return persons[0].first;
        } else {
            return min_dist.second;
        }
    } else {
        std::vector<std::pair<int64_t, int64_t>> vec_id_times(id_times.begin(), id_times.end());
        std::sort(vec_id_times.begin(), vec_id_times.end(), [](std::pair<int64_t, int64_t> const &x, std::pair<int64_t, int64_t> const &y) -> bool {
            if (x.second > y.second) {
                return true;
            } else if (x.second == y.second && x.first > y.first) {
                return true;
            }
            return false;
        });
        return vec_id_times[0].first;
    }
    return -1;
}
} // ns_uestc_vhm