#include "httpclient.h"

#include <iostream>
#include <sstream>

#include "httppayload.h"

namespace secplugs
{
	httpclient::httpclient()
	{
		hSession = nullptr;
		hConnection = nullptr;
		hRequest = nullptr;
		server = "";
		host = L"";
		port = 0;
		userAgent = L"secplugs/1.0";
		statusCode = 0;
		error = 0;
	}

	bool httpclient::init(bool reuse)
	{
		if (hSession && reuse)
		{
			return true;
		}
		// WinHttpCloseHandle(hRequest);
		// WinHttpCloseHandle(hConnection);
		// WinHttpCloseHandle(hSession);
		if (server.empty())
		{
			if (debugEnabled())
				std::cerr << "Server name not initialised" << '\n';
			return false;
		}
		if (host.empty())
		{
			std::wstring tmp(server.begin(), server.end());
			host = std::move(tmp);
		}
		hSession = WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession)
		{
			if (debugEnabled())
				std::cerr << "Init failed: " << GetLastError() << '\n';
			return false;
		}
		return true;
	}

	bool httpclient::init(const std::string& server_, int port_)
	{
		server = server_;
		port = port_;
		std::wstring tmp(server.begin(), server.end());
		host = std::move(tmp);
		return init();
	}

	bool httpclient::connect()
	{
		bool success = false;
		if (!hSession)
		{
			success = init(false);
		}
		if (!success && !hSession)
		{
			if (debugEnabled())
				std::cerr << "Init failed\n";
			return false;
		}
		if (debugEnabled())
			std::wcerr << "Initialised, connecting to " << host << "\n";
		hConnection = WinHttpConnect(hSession, host.c_str(), static_cast<INTERNET_PORT>(port), 0);
		if (hConnection)
		{
			success = true;
		}
		else
		{
			error = GetLastError();
			if (debugEnabled())
				std::cerr << "Error connecting: " << error << '\n';
			success = false;
		}
		return success;
	}

	bool httpclient::get(const map_str_str& headers, const std::string& path, std::string& response)
	{
		bool success = false;
		if (!hConnection)
		{
			success = connect();
		}

		if (success || (hConnection != nullptr))
		{
			std::wstring wPath(path.begin(), path.end());
			hRequest = WinHttpOpenRequest(hConnection, L"GET",
				wPath.c_str(), L"HTTP/1.1",
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
			if (hRequest)
			{
				if (add_request_headers(headers))
				{
					success = make_request();
					if (!success)
					{
						if (debugEnabled())
							std::cerr << "Sending request failed\n";
						return false;
					}
				}
				if (!success)
				{
					if (debugEnabled())
						std::cerr << "Adding request headers failed: " << error << '\n';
					return success;
				}
			}
			else
			{
				error = GetLastError();
				if (debugEnabled())
					std::cerr << "hRequest not created\n";
			}
		}
		else
		{
			if (debugEnabled())
				std::cerr << "Connection creation failed\n";
			return false;
		}
		if (debugEnabled())
			std::cout << "Sent request successfully ? " << success << "\n";
		map_str_str respHeaders;
		success = receive_response(statusCode, respHeaders, response);
		return success;
	}

	bool httpclient::post(const map_str_str& headers, const std::string& path, payload& data)
	{
		bool success = false;

		if (!hConnection)
		{
			success = connect();
		}

		if (success || (hConnection != nullptr))
		{
			std::wstring wPath(path.begin(), path.end());
			hRequest = WinHttpOpenRequest(hConnection, L"GET",
				wPath.c_str(), L"HTTP/1.1",
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
			if (hRequest)
			{
				if (add_request_headers(headers))
				{
					success = make_request(data);
					if (!success)
					{
						return false;
					}
				}
				if (!success)
				{
					if (debugEnabled())
						std::cerr << "Adding request headers failed: " << error << '\n';
					return success;
				}
			}
			else
			{
				error = GetLastError();
			}
		}
		return success;
	}


	bool httpclient::add_request_headers(const map_str_str& headers)
	{
		bool success = false;
		if (hRequest)
		{
			if (headers.empty())
			{
				return true;
			}
			for (auto& header : headers)
			{
				std::string tmp = header.first + ": " + header.second;
				std::wstring hdr(tmp.begin(), tmp.end());
				success = WinHttpAddRequestHeaders(hRequest, hdr.c_str(), static_cast<ULONG>(-1L), WINHTTP_ADDREQ_FLAG_ADD);
				if (!success)
				{
					error = GetLastError();
					if (debugEnabled())
						std::cerr << "Error adding request header: " << header.first << "::" << header.second << "\n";
					return false;
				}
			}
		}
		else
		{
			success = false;
			if (debugEnabled())
				std::cerr << "Request not initialised" << '\n';
		}
		return success;
	}

	bool httpclient::make_request(payload& data)
	{
		return make_request_with_data(data);
	}

	bool httpclient::make_request()
	{
		return make_request_no_data();
	}

	
	bool httpclient::make_request_no_data()
	{
		bool success = false;
		if (hRequest)
		{
			success = WinHttpSendRequest(hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				WINHTTP_NO_REQUEST_DATA,
				0,
				0,
				0);
			if (success)
			{
				if (debugEnabled())
					std::cout << "Request sent successfully\n";
				success = WinHttpReceiveResponse(hRequest, nullptr);
			}
			else
			{
				error = GetLastError();
				if (debugEnabled())
					std::cerr << "Error sending request without payload " << error << '\n';
			}
		}
		return success;
	}

	bool httpclient::make_request_with_data(payload& data)
	{
		bool success = false;
		if (hRequest)
		{
			auto contentTypeHdr = "Content-Type: " + data.getContentType() + "\r\n";
			std::wstring header(contentTypeHdr.begin(), contentTypeHdr.end());
			std::string body;
			data.toString(body);
			success = WinHttpSendRequest(hRequest,
				header.c_str(), header.size(),
				(LPVOID)body.c_str(), body.size(),
				body.size(), NULL);
			if (success)
			{
				success = WinHttpReceiveResponse(hRequest, nullptr);
			}
			else
			{
				error = GetLastError();
			}
		}
		return success;
	}

	bool httpclient::receive_response(DWORD& statusCode, map_str_str& headers, std::string& response)
	{
		bool success = false;
		if (!hRequest)
		{
			if (debugEnabled())
				std::cerr << "Error receiving response, hRequest is not defined " << '\n';
			return success;
		}
		success = receive_response_headers(statusCode, headers);
		if (success)
		{
			success = receive_response_body(response);
		}
		else
		{
			if (debugEnabled())
				std::cerr << "Error receiving response headers: " << error << '\n';
		}
		if (debugEnabled())
			std::cerr << "receive_response:: Success ? " << success << "\n";
		return success;
	}

	bool httpclient::receive_response_headers(DWORD& statusCode, map_str_str& headers)
	{
		bool success = false;
		DWORD statusSize = sizeof(statusCode);
		DWORD headerSize;
		if (!hRequest)
		{
			if (debugEnabled())
				std::cerr << "Request is undefined, failure" << '\n';
			return false;
		}
		success = WinHttpQueryHeaders(hRequest,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			nullptr, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);
		if (success)
		{
			if (debugEnabled())
				std::cout << "Received HTTP Status: " << statusCode << '\n';
			success = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, nullptr, nullptr, &headerSize, WINHTTP_NO_HEADER_INDEX);
			if (!success && (GetLastError() == ERROR_INSUFFICIENT_BUFFER))
			{
				auto respHeaders = new char[headerSize];
				success = WinHttpQueryHeaders(hRequest,
					WINHTTP_QUERY_RAW_HEADERS_CRLF,
					nullptr, respHeaders, &headerSize, WINHTTP_NO_HEADER_INDEX);
				if (!success)
				{
					error = GetLastError();
				}
				else
				{
					success = true;
					headers.clear();
				}
			}
		}
		return success;
	}

	bool httpclient::receive_response_body(std::string& response)
	{
		bool success = true;
		DWORD respSize, downloaded;
		if (!hRequest)
		{
			return false;
		}
		while (true)
		{
			respSize = 0;
			downloaded = 0;
			DWORD readSize = 0;
			success = WinHttpQueryDataAvailable(hRequest, &respSize);
			if (!success)
			{
				error = GetLastError();
				break;
			}
			if (respSize == 0)
			{
				success = true;
				break;
			}
			do
			{
				char* tmp;
				tmp = new char[respSize + 1];
				ZeroMemory(tmp, respSize + 1);
				success = WinHttpReadData(hRequest, tmp, respSize, &downloaded);
				if (!success)
				{
					error = GetLastError();
					break;
				}
				if (downloaded == 0)
				{
					success = true;
					break;
				}
				response += tmp;
				respSize -= downloaded;
			} while (respSize > 0);
			if (!success)
			{
				break;
			}
		}
		return success;
	}

}
