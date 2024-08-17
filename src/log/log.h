#ifndef _UESTC_VHM_LOG_H_
#define _UESTC_VHM_LOG_H_

#include <cstdio>
#include <string>
#include <time.h>

namespace ns_uestc_vhm {

static std::string GetSystemTime(std::string const &format) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char buffer[64];
    strftime(buffer, sizeof(buffer), format.c_str(), tm_info);
    return std::string(buffer);
}

#ifndef PRINT
#define PRINT(fmt, ...)                                                            \
    do {                                                                           \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]"); \
        fprintf(stdout, "%s " fmt, time_str.c_str(), ##__VA_ARGS__);               \
    } while (0)
#endif // PRINT

#ifndef PRINT_DEBUG
#define PRINT_DEBUG(fmt, ...)                                                          \
    do {                                                                               \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]");     \
        fprintf(stdout, "%s[UESTC_VHM][DEBUG] " fmt, time_str.c_str(), ##__VA_ARGS__); \
    } while (0)
#endif // PRINT_DEBUG

#ifndef PRINT_INFO
#define PRINT_INFO(fmt, ...)                                                          \
    do {                                                                              \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]");    \
        fprintf(stdout, "%s[UESTC_VHM][INFO] " fmt, time_str.c_str(), ##__VA_ARGS__); \
    } while (0)
#endif // PRINT_INFO

#ifndef PRINT_WARNING
#define PRINT_WARNING(fmt, ...)                                                          \
    do {                                                                                 \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]");       \
        fprintf(stdout, "%s[UESTC_VHM][WARNING] " fmt, time_str.c_str(), ##__VA_ARGS__); \
    } while (0)
#endif // PRINT_WARNING

#ifndef PRINT_ERROR
#define PRINT_ERROR(fmt, ...)                                                          \
    do {                                                                               \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]");     \
        fprintf(stdout, "%s[UESTC_VHM][ERROR] " fmt, time_str.c_str(), ##__VA_ARGS__); \
    } while (0)
#endif // PRINT_ERROR

#ifndef PRINT_FATAL
#define PRINT_FATAL(fmt, ...)                                                          \
    do {                                                                               \
        std::string time_str = ns_uestc_vhm::GetSystemTime("[%Y-%m-%d %H:%M:%S]");     \
        fprintf(stdout, "%s[UESTC_VHM][FATAL] " fmt, time_str.c_str(), ##__VA_ARGS__); \
    } while (0)
#endif // PRINT_FATAL

} // ns_uestc_vhm

#endif // _UESTC_VHM_LOG_H_
