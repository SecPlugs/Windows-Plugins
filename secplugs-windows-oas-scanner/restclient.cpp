#include "restclient.hpp"

namespace secplugs {

    /**
     *
     * @param cfg_ - A pre-constructed configloader object.
     *
     * This creates the SSL context that will be common for all HTTPS sessions.
     * For now, we do not validate the certificates.
     *
     * TODO: Add a config attr for CA path and validate server certificates.
     */
    restclient::restclient(const secplugs::configloader& cfg_) {
        cfg = cfg_;
        submission_needed = false;
    }

	void restclient::init_client()
    {
        //client.set_config(cfg);
        return;
    }

    /**
     *
     * @param file - The file to be submitted to secplugs to be scanned
     * @param scan_type - The type of scan - Can be SCORE, QUICKSCAN or DEEPSCAN
     * @return - Returns true if the file is non-malicious, false otherwise.
     *
     * This is the only method exposed by this class for callers. This internally handles
     * uploading the file to an S3 bucket and invoking the secplugs security APIs to get
     * score/scan results.
     */
    bool restclient::is_clean(const std::wstring& file) {
        std::string response;
    	return scan_file(file, response);
    }

    /**
     *
     * @param sha256 - The sha256 hash of the file being submitted for scan
     * @param scan_type - SCORE/QUICKSCAN/DEEPSCAN
     * @return - Returns true if the file is non-malicious, false otherwise.
     *
     * This method talks to the /file/<scantype> end point of the Secplugs security API.
     * For now, this handles only the "score" from the reponse.
     *
     * TODO: Add handlers for quick scan and deep scan results as well.
     */
    bool restclient::scan_file(const std::wstring& file_path, std::string& response) {
        JsonObjPtr ptr = new Poco::JSON::Object;
        /*
        quick_scan(file_path, ptr);
    	if (client.status == 200)
    	{
            const auto score = ptr->getValue<int>("score");
            std::cout << score << std::endl;
    		if (score < 70)
    		{
                response = ptr->getValue<std::string>("json_report");
                return false;
    		} else
    		{
                response = ptr->getValue<std::string>("json_report");
                return true;
    		}
    	}
    	else
        {
	        if (client.status == 404)
	        {
		        */
		        sha256 = common::compute_sha256(file_path);
		        JsonObjPtr uploadInfo = new Poco::JSON::Object;
		        auto status = get_presigned_url(uploadInfo);
		        if (status)
		        {
			        // std::cout << "Uploading file" << std::endl;
                    auto uploadMeta = uploadInfo->getObject("upload_post");
                    if (upload_file(file_path, uploadMeta)) {
                        quick_scan(file_path, ptr);
                        response = ptr->getValue<std::string>("json_report");
                    	if (ptr->getValue<int>("score") < 70)
                    	{
                            return false;
                    	}
                        else
                        {
                            return true;
                        }
                     //    if (client.getStatusCode() == 200)
                     //    {
                     //        std::cout << "ScanResult: " << ptr->getValue<int>("score") << std::endl;
                     //    }
                    	// else
                     //    {
                     //        std::cout << "ScanError: " << client.getStatusCode() << std::endl;
                     //    }
                    }
                    else
                    {
                        std::cout << "Upload file failed" << std::endl;
                        return false;
                    }
			        return true;
		        }
		        else
		        {
			        std::cout << "Could not upload file" << std::endl;
			        return false;
		        }
	        //}
	        // std::cout << "ScanError: " << client.status << std::endl;
	        // return false;
        //}
    }

    /**
     *
     * @param sha256 - The sha256 hash of the file that we want to upload to S3
     * @param ptr - The parameter that is populated by this method
     *
     * This method uses the /file/upload endpoint to get a pre-signed AWS URL using which
     * a file can be uploaded to S3. This endpoint returns the URL and several other HTTP params
     * that must be passed on to the URL along with the payload. The response is a JSON payload
     *
     */
    bool restclient::get_presigned_url(JsonObjPtr& ptr) {
        const Poco::URI uri(cfg.get_base_url());
        const auto uri_path = "/security/file/upload?sha256=" + sha256;
        client.init(uri.getHost(), 443);
        map_str_str reqHeaders;
    	reqHeaders.insert({ "x-api-key", cfg.get_api_key() });
        std::string response;
        const auto status = client.get(reqHeaders, uri_path, response);
        // std::cout << "HTTP Response: " << client.getStatusCode() << "\n";
        // std::cout << "Error Code: " << client.getErrorCode() << "\n";
        if (status)
        {
            check_and_get_json(response, ptr);
            return true;
        }
        else
        {
            std::cerr << "Failed to get presigned url\n";
            return false;
        }
    	
    }

    /**
     *
     * @param json - The JSON produced by the get_presigned_url call.
     * @return url - The URL property of the JSON that is passed as input.
     */
    std::string restclient::get_url(JsonObjPtr json) {
        if (json->has("url")) {
            return json->getValue<std::string>("url");
        }
        else {
            std::cerr << "No URL!!" << '\n';
        }
        return "";
    }

    /**
     *
     * @param json -  The JSON produced by the get_presigned_url call.
     * @param fields - The "fields" property of the input JSON converted to a map.
     *
     */
    void restclient::get_fields(JsonObjPtr json, map_str_str& fields) {
        auto fieldsObj = json->getObject("fields");
        for (auto& itr : *fieldsObj)
        {
            fields.insert(std::pair<std::string, std::string>(itr.first, itr.second));
        }
    }

    /**
     *
     * @param file - The path of the file to be uploaded
     * @param sha256 - The sha256 has of the file to be uploaded
     * @return - Indicates whether the upload was successful or not
     */
    bool restclient::upload_file(const std::wstring& file, JsonObjPtr& uploadInfo) {
        std::string upload_uri = get_url(uploadInfo);
        map_str_str fields;
        get_fields(uploadInfo, fields);
        Poco::URI uri(upload_uri);
        client.init(uri.getHost(), 443);
        map_str_str reqHeaders;
        payload data;
    	for (auto& field: fields)
    	{
            data.addFormField(field.first, field.second);
    	}
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        data.addFile(converter.to_bytes(file));
        client.post(reqHeaders, uri.getPath(), data);
	    return true;
    }

    /**
     *
     * @param json - The JSON response from the Secplugs file upload API
     * @return - The "score" property from the JSON response
     *
     */
    int restclient::extractScore(const std::string& json) {
        Poco::JSON::Parser parser;
        auto tmp = parser.parse(json);
        int score = tmp.extract<Poco::JSON::Object::Ptr>()->getValue<int>("score");
        return score;
    }

	/**
	 *
	 * @param file - The file to be scanned
	 * @return - True if malicious, false otherwise
	 */

	bool restclient::proxy_scan (const std::wstring& file)  const {
        Poco::Net::initializeSSL();
        Poco::Net::SSLManager::InvalidCertificateHandlerPtr handlerPtr(new Poco::Net::AcceptCertificateHandler(false));
        const Poco::Net::Context::Ptr ctxt(new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", Poco::Net::Context::VERIFY_NONE, 11));
        Poco::Net::SSLManager::instance().initializeClient(nullptr, handlerPtr, ctxt);
        //std::cout << ctxt->sessionCacheEnabled() << '\n';
        //Poco::Net::SSLManager::instance().initializeClient(nullptr, handlerPtr, ctxt);
        std::string uploadUrl = SECPLUGS_PROXY_URL;
        std::string fullUrl("https://" + uploadUrl + "/file/upload");
        Poco::URI uri(fullUrl);
        Poco::Net::HTTPSClientSession sp_proxy(uploadUrl, 443, ctxt);
        //Poco::Net::HTTPSClientSession sp_proxy(ctxt);
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, uri.getPath());
        req.add("x-api-key", cfg.get_api_key());
        req.add("Host", uri.getHost());
        req.setVersion("HTTP/1.1");
        Poco::Net::HTMLForm form;
        form.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
        std::string file_name_ascii(file.begin(), file.end());
        form.addPart("file", new Poco::Net::FilePartSource(file_name_ascii, file_name_ascii, "application/octetstream"));
    	form.prepareSubmit(req);
     //    std::fstream ofstr("d:/temp/outdata.log", std::ios::out);
     //    req.write(ofstr);
     //    form.write(ofstr);
     //    ofstr.close();
    	// std::fstream ifstr("d:/temp/outdata.log", std::ios::in);
     //    std::string ss;
     //    Poco::StreamCopier::copyToString(ifstr, ss);
     //    std::cout << ss << "This is done" << std::endl;
        form.write(sp_proxy.sendRequest(req));
		Poco::Net::HTTPResponse res;
		std::istream& res_stream = sp_proxy.receiveResponse(res);
		std::string responseJson;
		Poco::StreamCopier::copyToString(res_stream, responseJson);
		std::cout << responseJson << std::endl;
        return false;
	}



	bool restclient::quick_scan(const std::wstring& file_path, JsonObjPtr& result)
    {
        const Poco::URI uri(cfg.get_base_url());
        const auto sha256 = common::compute_sha256(file_path);
        const auto uri_path = "/security/file/quickscan?sha256=" + sha256;
        client.init(uri.getHost(), 443);
        map_str_str reqHeaders;
        reqHeaders.insert({ "x-api-key", cfg.get_api_key() });
        std::string response;
        // std::cout << "Querying: " << uri_path << std::endl;
        const auto status = client.get(reqHeaders, uri_path, response);
        if (status)
        {
            check_and_get_json(response, result);
            return true;
        } else
        {
        	return false;
        }
    	
    }

	bool restclient::check_and_get_json(const std::string& res, JsonObjPtr& json)
	{
    	try
    	{
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var response = parser.parse(res);
            json = response.extract<JsonObjPtr>();
            return true;
    	} catch (...)
    	{
            return false;
    	}
	}

}
