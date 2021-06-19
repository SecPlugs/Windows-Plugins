#ifndef SECPLUGS_COMMON_RESTCLIENT_H
#define SECPLUGS_COMMON_RESTCLIENT_H

#include <string>
#include <Poco/JSON/JSON.h>
#include <Poco/Net/Context.h>
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/URI.h"
#include "Poco/Environment.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NetException.h"
#include "Poco/URI.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "winhttp.h"
#include "configloader.h"
#include "common.h"
#include "httpclient.h"
#include "httppayload.h"

using JsonObjPtr = Poco::JSON::Object::Ptr;
using map_str_str = std::map<std::string, std::string>;

namespace secplugs {
    class restclient {
    public:
        explicit restclient(const secplugs::configloader& cfg_);
        bool is_clean(const std::wstring& file);
        void init_client();
        bool scan_file(const std::wstring& file, std::string& response);
        bool quick_scan(const std::wstring& file, JsonObjPtr& result);
    private:
        bool upload_file(const std::wstring& filepath, JsonObjPtr& uploadInfo);
        bool proxy_scan(const std::wstring& file) const;
        bool get_presigned_url(JsonObjPtr& ptr);
        static std::string get_url(JsonObjPtr json);
        static void get_fields(JsonObjPtr json, map_str_str& fields);
        static int extractScore(const std::string& json);
        bool check_and_get_json(const std::string& res, JsonObjPtr& json);
        secplugs::configloader cfg;
        secplugs::httpclient client;
        bool submission_needed = false;
        std::string sha256 = "";
    };
}

#endif

