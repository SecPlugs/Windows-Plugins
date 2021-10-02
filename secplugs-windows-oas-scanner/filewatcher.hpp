#ifndef SECPLUGS_COMMON_FILEWATCHER_H
#define SECPLUGS_COMMON_FILEWATCHER_H

#include <string>
#include <vector>
#include <map>
#include <tchar.h>
#include <iostream>
#include "restclient.hpp"
#include <Windows.h>

namespace secplugs {
	class filewatcher
	{
	public:
		void add_directory(const std::string& path);
		void watch(secplugs::restclient& rest);
	private:
		void reload_dir(LPCWSTR dir);
		void watch_dir(const std::wstring& dir, restclient& rest);
		
		std::wstring defaultWatchPath = _T("C:\\Windows");
		std::vector<std::wstring> watchDirs;
	};

}

#endif
