#ifndef SECPLUGS_COMMON_CONFIGLOADER_H
#define SECPLUGS_COMMON_CONFIGLOADER_H
#define  _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <utility>
#include <fstream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <Poco/JSON/Parser.h>

#define SECPLUGS_API_URL "https://api.live.secplugs.com/security"
#define SECPLUGS_PROXY_URL "proxy.secplugs.com"
#define DEFAULT_API_KEY "r2iKI4q7Lu91Nu5uPl3eW3BPmRo4XK1ZbhLWtOKd"

using stringpair_t = std::pair<std::string, std::string>;

namespace secplugs {
    class configloader {
    public:
        explicit configloader(std::string  file);
        configloader() = default;
        void load_config(std::string file);
        [[nodiscard]] inline std::string get_api_key() const { return api_key; }
        [[nodiscard]] inline std::string get_base_url() const { return base_url; }
        [[nodiscard]] inline std::vector<std::string> get_watchers() const { return watchers; }
        [[nodiscard]] inline std::string get_client_cert() const { return client_cert; }
        [[nodiscard]] inline std::string get_scheme() const { return scheme; }

    private:
        std::string config_file;
        std::string api_key;
        std::vector<std::string> watchers;
        std::string base_url;
        std::string scheme = "https://";
        std::map<std::string, std::string> proxy;
        std::string client_cert;
    };
}

#endif

