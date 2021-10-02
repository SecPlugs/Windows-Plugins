#include <iomanip>
#include "common.h"

namespace secplugs {
    std::string common::compute_sha256(const std::wstring& file) {
        SHA256_CTX sha_ctx;
        if (!SHA256_Init(&sha_ctx)) {
            return "";
        }
        char buf[1024 * 16];
        std::fstream ifs(file, std::ios::in | std::ios::binary);
        while (ifs.good()) {
            ifs.read(buf, 1024 * 16);
            if (!SHA256_Update(&sha_ctx, buf, ifs.gcount())) {
                return "";
            }
        }
        unsigned char result[SHA256_DIGEST_LENGTH];
        if (!SHA256_Final(result, &sha_ctx)) {
            return "";
        }
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (const auto& byte : result) {
            ss << std::setw(2) << (int)byte;
        }
        return ss.str();
    }
}