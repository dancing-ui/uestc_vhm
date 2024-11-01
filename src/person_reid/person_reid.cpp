#include "person_reid.h"
#include "base64.h"
#include "cstring.h"
#include "database.h"
#include <algorithm>
#include <cstddef>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>
#include "database.h"
#include "model_handle_common.h"
#include "utils.h"
#include "log.h"

namespace ns_uestc_vhm {

void PersonReidCtx::SetCfg(PersonReidParameter const &cfg) {
    cfg_ = cfg;
}

int32_t PersonReidCtx::Reid(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists) {
    int32_t ret{0};
    assert(detect_boxes.size() == feats_lists.size());
    assert(imgs_batch.size() == feats_lists.size());
    database_ = dbase::DataBases::instance()->at(cfg_.database_param_.db_name);
    reid_boxes_lists_.clear();

    for (size_t i = 0; i < feats_lists.size(); i++) {
        std::vector<TrackerRes> reid_boxes;
        std::vector<std::string> b64_features;
        for (size_t j = 0; j < feats_lists[i].size(); j++) {
            cv::Mat feat = feats_lists[i][j].clone();
            std::string b64_feat{convertMat2B64(feat)};
            b64_features.emplace_back(b64_feat);
        }

        std::vector<std::vector<dbase::Recall>> all_recalls{database_->batch_search(b64_features, cfg_.top_k)};

        std::vector<std::string> cids, fids;
        std::vector<std::string> add_b64_features;
        for (size_t j = 0; j < feats_lists[i].size(); j++) {
            std::string fid;
            if (all_recalls[j].empty()) {
                fid = database_->GenerateFeatureID();
                cids.push_back(cid_);
                fids.push_back(fid);
                add_b64_features.push_back(b64_features[j]);
            } else {
                std::string key{all_recalls[j][0].cfid};
                std::vector<std::string> split_str;
                utils::SplitString(key, split_str, DELIMITER);
                assert(split_str.size() == 2);
                assert(cid_ == split_str[0]);
                fid = split_str[1];
            }
            TrackerRes reid_res{detect_boxes[i][j].label, detect_boxes[i][j].confidence,
                                detect_boxes[i][j].left, detect_boxes[i][j].top,
                                detect_boxes[i][j].width, detect_boxes[i][j].height,
                                utils::StrToInt64(fid)};
            reid_boxes.emplace_back(reid_res);
        }
        reid_boxes_lists_.emplace_back(reid_boxes);
        ret = database_->batch_add_and_save(cids, fids, add_b64_features);
        if (ret < 0) {
            PRINT_ERROR("PersonReidCtx::Reid batch_add_and_save failed, ret=%d\n", ret);
            return -1;
        }
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

} // ns_uestc_vhm