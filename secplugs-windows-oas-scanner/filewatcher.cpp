#include "filewatcher.hpp"

#include <thread>

void secplugs::filewatcher::add_directory(const std::string& path)
{
	std::wstring watchDir(path.begin(), path.end());
	watchDirs.push_back(watchDir);
}

void secplugs::filewatcher::watch(restclient& rest)
{
	std::vector<std::thread> threads;
	for(auto& dir: watchDirs)
	{
		std::thread t(&filewatcher::watch_dir, this, std::ref(dir), std::ref(rest));
		threads.push_back(std::move(t));
	}
	for(auto& thread: threads)
	{
		thread.join();
	}
}

void secplugs::filewatcher::watch_dir(const std::wstring& dir, secplugs::restclient& rest)
{
	HANDLE dirHandles[2] = { nullptr, nullptr };
	dirHandles[0] = FindFirstChangeNotification(dir.c_str(),
		true,
		FILE_NOTIFY_CHANGE_DIR_NAME);
	dirHandles[1] = FindFirstChangeNotification(dir.c_str(),
		true,
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
	// std::wcout << L"Watching " << dir << std::endl;
	FILE_NOTIFY_INFORMATION file_notify_buffer[512]; //= new FILE_NOTIFY_INFORMATION[512];
	FILE_NOTIFY_INFORMATION dir_notify_buffer[512]; //= new FILE_NOTIFY_INFORMATION[512];
	while(true)
	{
		auto status = WaitForMultipleObjects(2, dirHandles, false, INFINITE);
		DWORD bytesReturned = 0;
		std::wstring path_with_changes;
		
		switch(status)
		{
		case WAIT_OBJECT_0:
			// Dir creation, log and ignore.
			// reload_dir(defaultWatchPath.c_str());
			ReadDirectoryChangesW(dirHandles[0], (LPVOID)&dir_notify_buffer, 1024, false, FILE_NOTIFY_CHANGE_ATTRIBUTES, &bytesReturned, NULL, NULL);
			std::cout << bytesReturned << " bytes returned" << '\n';
			// std::wcout << "WO0::" << dir_notify_buffer[0].FileName << '\n';
			FindNextChangeNotification(dirHandles[0]);
			break;

		case WAIT_OBJECT_0 + 1:
			ReadDirectoryChangesW(dirHandles[1], (LPVOID)&file_notify_buffer, 1024, false, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES, &bytesReturned, NULL, NULL);
			path_with_changes = dir + L"\\" + file_notify_buffer[0].FileName;
			// std::wcout << path_with_changes << " got modified" << '\n';
			if (std::experimental::filesystem::is_regular_file(path_with_changes))
			{
				if (!rest.is_clean(path_with_changes))
				{
					// std::cout << "TODO:: Remove file!" << '\n';
					std::experimental::filesystem::remove(path_with_changes);
				}
			}
			// else
			// {
			// 	std::cout << "Clean file!!" << '\n';
			// }

			FindNextChangeNotification(dirHandles[1]);
			break;

		case WAIT_TIMEOUT:
			break;

		default:
			// LPCTSTR buf = nullptr;
			auto err_id = GetLastError();
			// FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			// 			NULL, err_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			// 	(LPWSTR)&buf, 0, NULL);
			// OutputDebugString(buf);
			std::cout << "Error Done" << '\n';
			ExitProcess(err_id);
		}
				
	}
}

void secplugs::filewatcher::reload_dir(LPCWSTR dir)
{
	// TODO: How do we reload this dir to monitor changes in new sub-dir ?
}

