#include "Memory_Manager.h"

std::uintmax_t MemoryManager::getFileSize(std::string fPath)
{
	std::uintmax_t size = std::filesystem::file_size(fPath);
	return size;
}

bool MemoryManager::ifHugeFile(std::string path)
{
	loadGlobalMemStatusEx();
	if (getFileSize(path) > maxMemUse)
	{
		return true;
	}
	return false;
}

void MemoryManager::loadGlobalMemStatusEx()
{
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	maxMemUse = statex.ullTotalPhys;
	maxMemUse *= 2;
	maxMemUse /= 3;		//70% mem load
}