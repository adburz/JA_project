#pragma once
#include <windows.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

class MemoryManager
{
public:
	MemoryManager();

	DWORDLONG maxMemUse;
	std::uintmax_t bmp_fileSize;
	std::uintmax_t msg_fileSize;

	void loadGlobalMemStatusEx();
	bool isEnoughSpace(std::string bmp_path, std::string msg_path, bool pType, unsigned int mLength, int spaceInBmp);
	std::uintmax_t loadFileSize(std::string fPath);

private:
	MEMORYSTATUSEX statex;
};
