#pragma once
#include <windows.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

class MemoryManager
{
public:
	MemoryManager() {};

	void loadGlobalMemStatusEx();

	bool isEnoughSpace(std::string bmp_path, std::string msg_path, bool pType, unsigned int mLength);
	DWORDLONG maxMemUse;
	std::uintmax_t bmp_fileSize;
	std::uintmax_t msg_fileSize;

	DWORDLONG usingMem;
	DWORDLONG maxBMP_size;
	DWORDLONG maxMsg_size;

	std::ifstream input;
	std::ofstream output;

	std::uintmax_t current_fileSize = 0;
	std::uintmax_t current_loaded = 0;

	DWORDLONG BMP_loadSize;
	DWORDLONG Msg_loadSize;
	DWORDLONG loopCounter;


	std::uintmax_t loadFileSize(std::string fPath);
private:
	MEMORYSTATUSEX statex;

};
