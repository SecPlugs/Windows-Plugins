#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace secplugs
{
	enum http_payload_type
	{
		FormField,
		File
	};

	class part
	{
	public:
		part(std::string name_, std::string data_) : name(std::move(name_)), data(std::move(data_)) {}
		part(std::string fn_) : name("file"), fileName(std::move(fn_)) {}
		inline void setFileName(std::string fn_) { fileName = std::move(fn_); }
		inline void setType(http_payload_type type) { payloadType = type; }
		void toString(std::string& payload) const;
	private:
		std::string name;
		std::string data;
		std::string fileName;
		http_payload_type payloadType;
	};

	class payload
	{
	public:
		payload();
		inline void setContentType(const std::string& ctype_) { contentType = ctype_; }
		inline void setContentBoundary(const std::string& boundary_) { contentBoundary = boundary_; }
		void addFormField(const std::string& key, const std::string& data);
		void addFile(const std::string& filePath);
		void toString(std::string& string);
		inline std::string getContentType() const {
			return contentType;
		}
	private:
		std::string contentType;
		std::vector<part> parts;
		std::string contentBoundary;
	};

}
