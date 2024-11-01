#ifndef _UESTC_VHM_BASE64_H_
#define _UESTC_VHM_BASE64_H_

#include <vector>
#include <string>

namespace ns_uestc_vhm {
namespace utils {

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64_encode(unsigned char const *, unsigned int len);
std::string base64_decode(std::string const &s);

void Base64Encode(std::vector<float> const &vec, std::string &s);
void Base64Decode(std::string const &s, std::vector<float> &vec);

} // utils
} // ns_uestc_vhm

#endif /* ifndef _UESTC_VHM_BASE64_H_ */
