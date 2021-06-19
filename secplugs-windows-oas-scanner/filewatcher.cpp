#include "filewatcher.hpp"

void secplugs::filewatcher::add_directory(const std::string& path)
{
}

void secplugs::filewatcher::init()
{
	dirHandles[0] = FindFirstChangeNotification(defaultWatchPath.c_str(),
												true,
												FILE_NOTIFY_CHANGE_DIR_NAME);
	dirHandles[1] = FindFirstChangeNotification(defaultWatchPath.c_str(),
												true,
												FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
	
}

void secplugs::filewatcher::watch(secplugs::restclient& rest)
{
	while(true)
	{
		auto status = WaitForMultipleObjects(2, dirHandles, false, INFINITE);
		DWORD bytesReturned = 0;
		FILE_NOTIFY_INFORMATION file_notify_buffer[1024] = {};
		FILE_NOTIFY_INFORMATION dir_notify_buffer[1024] = {};
		std::wstring path_with_changes;
		
		switch(status)
		{
		case WAIT_OBJECT_0:
			// Dir creation, log and ignore.
			// reload_dir(defaultWatchPath.c_str());
			ReadDirectoryChangesW(dirHandles[0], (LPVOID)&dir_notify_buffer, 1024, false, FILE_NOTIFY_CHANGE_ATTRIBUTES, &bytesReturned, NULL, NULL);
			std::cout << bytesReturned << " bytes returned" << '\n';
			std::wcout << "WO0::" << dir_notify_buffer[0].FileName << '\n';
			FindNextChangeNotification(dirHandles[0]);
			break;

		case WAIT_OBJECT_0 + 1:
			ReadDirectoryChangesW(dirHandles[1], (LPVOID)&file_notify_buffer, 1024, false, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES, &bytesReturned, NULL, NULL);
			path_with_changes = defaultWatchPath + L"\\" + file_notify_buffer[0].FileName;
			//std::wcout << path_with_changes << " got modified" << '\n';
			if (std::experimental::filesystem::is_regular_file(path_with_changes))
			{
				if (!rest.is_clean(path_with_changes))
				{
					std::cout << "TODO:: Remove file!" << '\n';
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
			LPCTSTR buf = nullptr;
			auto err_id = GetLastError();
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, err_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&buf, 0, NULL);
			OutputDebugString(buf);
			std::cout << "Error Done" << '\n';
			ExitProcess(err_id);
		}
				
	}
}

void secplugs::filewatcher::reload_dir(LPCWSTR dir)
{
	// TODO: How do we reload this dir to monitor changes in new sub-dir ?
}

