#include "utils.h"

namespace ns_uestc_vhm {

std::vector<unsigned char> utils::loadModel(const std::string &file) {
    std::ifstream in(file, std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        return {};
    }
    in.seekg(0, std::ios::end);
    size_t length = in.tellg();

    std::vector<uint8_t> data;
    if (length > 0) {
        in.seekg(0, std::ios::beg);
        data.resize(length);
        in.read((char *)&data[0], length);
    }
    in.close();
    return data;
}

utils::HostTimer::HostTimer() {
    t1 = std::chrono::steady_clock::now();
}

float utils::HostTimer::getUsedTime() {
    t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_used = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    return (1000 * time_used.count()); // ms
}

utils::HostTimer::~HostTimer() {
}

utils::DeviceTimer::DeviceTimer() {
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start);
}

float utils::DeviceTimer::getUsedTime() {
    cudaEventRecord(end);
    cudaEventSynchronize(end);
    float total_time;
    cudaEventElapsedTime(&total_time, start, end);
    return total_time;
}

utils::DeviceTimer::DeviceTimer(cudaStream_t stream) {
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start, stream);
}

float utils::DeviceTimer::getUsedTime(cudaStream_t stream) {
    cudaEventRecord(end, stream);
    cudaEventSynchronize(end);
    float total_time;
    cudaEventElapsedTime(&total_time, start, end);
    return total_time;
}

utils::DeviceTimer::~DeviceTimer() {
    cudaEventDestroy(start);
    cudaEventDestroy(end);
}

bool utils::CheckFloatEqual(float f_x, float f_y) {
    return std::fabs(f_x - f_y) < std::numeric_limits<float>::epsilon();
}

int64_t utils::StrToInt64(std::string const &str) {
    int64_t res{0};
    std::stringstream ss;
    ss << std::hex << str;
    ss >> res;
    return res;
}

std::string utils::Int64ToStr(int64_t int64_x) {
    std::stringstream ss;
    ss << std::hex << int64_x;
    return ss.str();
}

std::string utils::LocalTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string year = std::to_string(1900 + ltm->tm_year);
    std::string month = std::to_string(1 + ltm->tm_mon);
    std::string day = std::to_string(ltm->tm_mday);
    std::string hour;
    std::string min;
    std::string sec;
    if (ltm->tm_hour + 8 < 10) // GMT to local hour, +8 hour
        hour = "0" + std::to_string(ltm->tm_hour + 8);
    else
        hour = std::to_string(ltm->tm_hour + 8);

    if (ltm->tm_min < 10)
        min = "0" + std::to_string(ltm->tm_min);
    else
        min = std::to_string(ltm->tm_min);

    if (ltm->tm_sec < 10)
        sec = "0" + std::to_string(ltm->tm_sec);
    else
        sec = std::to_string(ltm->tm_sec);

    std::string date;
    date = year + "-" + month + "-" + day + " " + hour + ":" + min + ":" + sec;
    return date;
}

} // ns_uestc_vhm