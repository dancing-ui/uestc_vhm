#ifndef _UESTC_VHM_MY_STRING_CVT_H_
#define _UESTC_VHM_MY_STRING_CVT_H_

#include <vector>
#include <opencv2/opencv.hpp>

namespace ns_uestc_vhm {
namespace utils {

using byte = unsigned char;

typedef union {
    float f;
    unsigned char c[sizeof(float)];
} UnionFloatBytes;

void SplitString(const std::string &s, std::vector<std::string> &v, const std::string &c);

template <typename T>
void VectorToStr(const std::vector<T> &vec, std::string &str) {
    str = std::to_string(vec[0]);
    for (uint i = 1; i < vec.size(); ++i)
        str = str + " " + std::to_string(vec[i]);
}

template <typename _Tp>
std::vector<_Tp> convertMat2Vector(const cv::Mat &mat) {
    return (std::vector<_Tp>)(mat.reshape(1, 1)); // 通道数不变，按行转为一行
}

template <typename _Tp>
cv::Mat convertVector2Mat(std::vector<_Tp> v, int channels, int rows) {
    cv::Mat mat = cv::Mat(v);                           // 将vector变成单列的mat
    cv::Mat dest = mat.reshape(channels, rows).clone(); // PS：必须clone()一份，否则返回出错
    return dest;
}

void StrToVector(const std::string &str, std::vector<float> &vec);

void VectorToBytes(const std::vector<float> &vec, std::vector<byte> &byte_array);
void VectorToBytes(const std::vector<float> &vec, std::string &byte_array);
void BytesToVector(const std::vector<byte> &byte_array, std::vector<float> &vec);
void BytesToVector(const std::string &byte_array, std::vector<float> &vec);

} // utils
} // ns_uestc_vhm
#endif // ifndef _UESTC_VHM_MY_STRING_CVT_H_
