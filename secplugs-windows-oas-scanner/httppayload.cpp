#include "httppayload.h"

#include <fstream>

namespace secplugs
{
	payload::payload()
	{
		parts.clear();
		contentBoundary = "secplugs-http-content-boundary-20210601000000";
		contentType = "multipart/form-data;boundary = " + contentBoundary;
		contentBoundary = "--" + contentBoundary;
	}

	void payload::addFormField(const std::string& key_, const std::string& data_)
	{
		part formData(key_, data_);
		formData.setType(FormField);
		parts.push_back(formData);
	}

	void payload::addFile(const std::string& filePath)
	{
		part fileData(filePath);
		fileData.setType(File);
		parts.push_back(fileData);
	}

	void payload::toString(std::string& httpPayload)
	{
		std::stringstream payloadStream;
		for (const auto& part : parts)
		{
			payloadStream << contentBoundary << "\r\n";
			std::string payloadPart;
			part.toString(payloadPart);
			payloadStream << payloadPart;
		}
		payloadStream << contentBoundary;
		payloadStream << "--\r\n";
		httpPayload = payloadStream.str();
	}

	void part::toString(std::string& payload) const
	{
		std::stringstream payloadStream;
		if (payloadType == FormField)
		{
			payloadStream << "Content-Disposition: form-data; name=\"" << name << "\"\r\n\r\n";
			payloadStream << data << "\r\n";
		}
		else if (payloadType == File)
		{
			payloadStream << "Content-Disposition: form-data; name=\"" << name << "\"; filename=\"" << fileName << "\"\r\n";
			payloadStream << "Content-Type: application/octetstream\r\n\r\n";
			std::ifstream ifs;
			ifs.open(fileName, std::ios::binary);
			if (ifs.good())
			{
				ifs.seekg(0, std::ios::end);
				auto fileSize = ifs.tellg();
				ifs.seekg(0);
				int bytesToRead = fileSize;
				while (bytesToRead > 0 && ifs.good())
				{
					char buffer[8192];
					auto toRead = (bytesToRead >= 8192) ? 8192 : bytesToRead;
					ifs.read(buffer, toRead);
					auto bytesRead = ifs.gcount();
					if (bytesRead < 8192)
						buffer[bytesRead] = '\0';
					payloadStream << buffer;
					bytesToRead -= toRead;
				}
			}
			payloadStream << "\r\n";
		}
		payload = payloadStream.str();
	}

}
