#ifndef _UESTC_VHM_PARAMETER_H_
#define _UESTC_VHM_PARAMETER_H_

#include "common_include.h"

namespace ns_uestc_vhm {

struct InitParameter {
    int num_class{80}; // coco
    std::vector<std::string> class_names;
    std::vector<std::string> input_output_names;

    bool dynamic_batch{true};
    size_t batch_size{1};
    int src_h, src_w;
    int dst_h, dst_w;

    float scale{255.f};
    float means[3] = {0.f, 0.f, 0.f};
    float stds[3] = {1.f, 1.f, 1.f};

    float iou_thresh{0.45f};
    float conf_thresh{0.25f};

    int topK{1000};
    std::string save_path;

    std::string winname{"TensorRT-Alpha"};
    int char_width = 11;
    int det_info_render_width = 15;
    double font_scale = 0.6;
    bool is_show{false};
    bool is_save{false};
    std::string model_path;
};
struct StreamMediaCfgItem {
    int32_t id;
    bool is_enable;
    std::string type;
    std::string name;
    std::string in;
    std::string out;
    int32_t dst_width;
    int32_t dst_height;
};

struct ReidParameter {
    std::string engine_path;
    int32_t input_width;
    int32_t input_height;
    int32_t output_size;
    int32_t device_id;
    int32_t batch_size;
};

struct DeepSortParameter {
    int32_t max_age;
    float iou_threshold;
    float sim_threshold;
    bool agnostic;
};

struct ObjectTrackParameter {
    DeepSortParameter deepsort_param;
};

struct ModelCfgItem {
    // yolo
    InitParameter param;
    // reid
    ReidParameter reid_param;
    // object track
    ObjectTrackParameter object_track_param;
};

struct ParamOpt {
    std::string config;
};

extern int32_t ParseOpt(int32_t argc, char **argv, ParamOpt &opt);

} // ns_uestc_vhm

#endif // _UESTC_VHM_PARAMETER_H_