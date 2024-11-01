#ifndef _UESTC_VHM_SINGLETON_H_
#define _UESTC_VHM_SINGLETON_H_

namespace ns_uestc_vhm {

#define DISABLE_COPY_AND_MOVE(Class)          \
    Class(const Class &) = delete;            \
    Class &operator=(const Class &) = delete; \
    Class(Class &&) = delete;                 \
    Class &operator=(Class &&) = delete;

#define SINGLETON_DECL(Class)                  \
public:                                        \
    static Class *instance();                  \
    static void exitInstance();                \
                                               \
private:                                       \
    DISABLE_COPY_AND_MOVE(Class)               \
    static Class *s_pInstance;                 \
    static std::once_flag s_instance_initFlag; \
    static std::mutex s_instance_mutex;

#define SINGLETON_IMPL(Class)                                                   \
    Class *Class::s_pInstance = nullptr;                                        \
    std::once_flag Class::s_instance_initFlag;                                  \
    std::mutex Class::s_instance_mutex;                                         \
    Class *Class::instance() {                                                  \
        std::call_once(s_instance_initFlag, []() { s_pInstance = new Class; }); \
        return s_pInstance;                                                     \
    }                                                                           \
    void Class::exitInstance() {                                                \
        std::lock_guard<std::mutex> lock(s_instance_mutex);                     \
        if (s_pInstance) {                                                      \
            delete s_pInstance;                                                 \
            s_pInstance = nullptr;                                              \
        }                                                                       \
    }

} // ns_uestc_vhm

#endif // _UESTC_VHM_SINGLETON_H_
