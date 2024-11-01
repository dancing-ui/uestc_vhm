#ifndef _UESTC_VHM_PERSON_REID_H_
#define _UESTC_VHM_PERSON_REID_H_

#include "database.h"
#include "parameter.h"
#include "utils.h"
#include "model_handle_common.h"
#include <opencv2/core/mat.hpp>
#include <vector>

namespace ns_uestc_vhm {

class PersonReidCtx {
public:
    PersonReidCtx() = default;
    ~PersonReidCtx() = default;

    PersonReidCtx(PersonReidCtx const &) = delete;
    PersonReidCtx &operator=(PersonReidCtx const &) = delete;
    PersonReidCtx(PersonReidCtx &&) = delete;
    PersonReidCtx &operator=(PersonReidCtx &&) = delete;

    int32_t Reid(std::vector<cv::Mat> const &imgs_batch, std::vector<std::vector<utils::Box>> const &detect_boxes, std::vector<std::vector<cv::Mat>> const &feats_lists);

    void SetCfg(PersonReidParameter const &cfg);
    std::vector<std::vector<TrackerRes>> GetReidBoxesLists() const;

private:
    std::string convertMat2B64(cv::Mat const &mat);

    PersonReidParameter cfg_;
    dbase::DataBase *database_;
    std::string cid_{"reid"}; // can not appear "_" character

    std::vector<std::vector<TrackerRes>> reid_boxes_lists_;
};

} // ns_uestc_vhm

#endif // _UESTC_VHM_PERSON_REID_H_