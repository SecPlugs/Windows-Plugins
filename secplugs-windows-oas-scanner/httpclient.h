#pragma once
#pragma comment(lib, "winhttp.lib")

#include <Windows.h>
#include <winhttp.h>
#include <string>
#include <locale>
#include <codecvt>
#include <map>

#include "httppayload.h"


namespace secplugs
{
	using map_str_str = std::map<std::string, std::string>;

	class httpclient
	{
	public:
		httpclient(std::string server_, int port_) : server(std::move(server_)), port(port_), host(L""), userAgent(L"secplugs/1.0") {}
		httpclient();
		bool init(const std::string& server_, int port_);
		bool init(bool reuse = false);
		bool connect();
		inline void setUserAgent(std::string ua_)
		{
			std::string ua = std::move(ua_);
			std::wstring wua(ua.begin(), ua.end());
			userAgent = std::move(wua);
		}
		inline void enableDebugging() { debug = true; }
		inline bool debugEnabled() const { return debug; }
		DWORD getStatusCode() const { return statusCode; }
		DWORD getErrorCode() const { return error; }
		bool get(const map_str_str& headers, const std::string& path, std::string& response);
		bool post(const map_str_str& headers, const std::string& path, payload& data);
	private:
		std::string server;
		std::wstring host;
		int port;
		std::wstring userAgent;
		HINTERNET hSession, hConnection, hRequest;
		bool connected = false;
		bool initialised = false;
		DWORD statusCode;
		DWORD error;
		bool debug = false;

		bool add_request_headers(const map_str_str& headers);
		bool make_request(payload& data);
		bool make_request();
		bool make_request_no_data();
		bool make_request_with_data(payload& data);
		bool receive_response(DWORD& statusCode, map_str_str& headers, std::string& response);
		bool receive_response_headers(DWORD& statusCode, map_str_str& headers);
		bool receive_response_body(std::string& response);
	};
}
