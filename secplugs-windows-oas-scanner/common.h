#ifndef SECPLUGS_COMMON_COMMON_H
#define SECPLUGS_COMMON_COMMON_H

#include <string>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <codecvt>


namespace secplugs {

    class common
	{
    public:
        static std::string compute_sha256(const std::wstring& file);
    };

    enum scantype {
        SCORE,
        QUICK_SCAN,
        DEEP_SCAN
    };

};

#endif
