#ifndef _UESTC_VHM_LOG_H_
#define _UESTC_VHM_LOG_H_

#include <cstdio>

namespace ns_uestc_vhm {

#ifndef PRINT
#define PRINT(fmt, args...) fprintf(stdout, fmt, ##args)
#endif // PRINT

#ifndef PRINT_DEBUG
#define PRINT_DEBUG(fmt, args...) fprintf(stdout, "[DEBUG] " fmt, ##args)
#endif // PRINT_DEBUG

#ifndef PRINT_INFO
#define PRINT_INFO(fmt, args...) fprintf(stdout, "[INFO] " fmt, ##args)
#endif // PRINT_INFO

#ifndef PRINT_WARNING
#define PRINT_WARNING(fmt, args...) fprintf(stdout, "[WARNING] " fmt, ##args)
#endif // PRINT_WARNING

#ifndef PRINT_ERROR
#define PRINT_ERROR(fmt, args...) fprintf(stderr, "[ERROR] " fmt, ##args)
#endif // PRINT_ERROR

#ifndef PRINT_FATAL
#define PRINT_FATAL(fmt, args...) fprintf(stderr, "[FATAL] " fmt, ##args)
#endif // PRINT_FATAL

} // ns_uestc_vhm


#endif // _UESTC_VHM_LOG_H_
