#pragma once
#include "Poco/Util/ServerApplication.h"
namespace secplugs
{
	class scannerservice : public Poco::Util::ServerApplication
	{
	public:
		explicit scannerservice(std::string cfgfile_) : configFile(std::move(cfgfile_)) {}
		int main(const std::vector<std::string>& args) override;
		void defineOptions(Poco::Util::OptionSet& options) override;
		void handleOption(const std::string& name, const std::string& value) override;
		void initialize(Poco::Util::ServerApplication& app);
	private:
		std::string configFile;
		
	};

}

