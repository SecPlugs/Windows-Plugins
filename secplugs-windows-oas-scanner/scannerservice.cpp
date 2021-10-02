#include "scannerservice.h"

#include "configloader.h"
#include "filewatcher.hpp"
#include "restclient.hpp"

namespace secplugs
{
	int scannerservice::main(const std::vector<std::string>& args)
	{
		// configloader cfg(this->config().getString("config"));
		configloader cfg("c:\\Program Files\\Secplugs\\config.json");
		restclient rest(cfg);
		filewatcher watcher;
		for (auto & dir: cfg.get_watchers())
		{
			watcher.add_directory(dir);
		}
		watcher.watch(rest);
		return 0;
	}

	void scannerservice::defineOptions(Poco::Util::OptionSet& options)
	{
		Poco::Util::ServerApplication::defineOptions(options);
		options.addOption(
			Poco::Util::Option("config", "c", "The path to the config file")
			.required(false)
			.repeatable(false)
			.argument("configfile", false)
		);
	}

	void scannerservice::handleOption(const std::string& name, const std::string& value)
	{
		Poco::Util::ServerApplication::handleOption(name, value);
		this->config().setString(name, value);
	}

	void scannerservice::initialize(Poco::Util::ServerApplication& app)
	{
		//this->config().setString("config", "C:\\Program Files\\secplugs\\config.json");
		this->config().setString("config", "D:\\temp\\config.json");
		this->loadConfiguration();
		Poco::Util::ServerApplication::initialize(app);
	}
}
