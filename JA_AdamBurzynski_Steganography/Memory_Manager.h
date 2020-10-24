#pragma once
#include <windows.h>
#include <cstdint>
#include <filesystem>
#include <string>

class MemoryManager
{
public:
	MemoryManager() {};

	void loadGlobalMemStatusEx();
	std::uintmax_t getFileSize(std::string fPath);
	bool ifHugeFile(std::string path);
private:
	MEMORYSTATUSEX statex;
	DWORDLONG maxMemUse;
};
