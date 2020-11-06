#include "Memory_Manager.h"
#define ENCODER true

MemoryManager::MemoryManager()
{
	maxMemUse = 0;
	bmp_fileSize = 0;
	msg_fileSize = 0;
}

std::uintmax_t MemoryManager::loadFileSize(std::string fPath)
{
	std::uintmax_t fileSize;
	try {
		fileSize = std::filesystem::file_size(fPath);
	}
	catch (std::exception e)
	{
		return 0;
	}
	return fileSize;
}

bool MemoryManager::isEnoughSpace(std::string bmp_path, std::string msg_path, bool pType, unsigned int mLength, int spaceInBmp)
{
	loadGlobalMemStatusEx();
	bmp_fileSize = loadFileSize(bmp_path);
	if (bmp_fileSize == 0)
	{
		return false;
	}

	if (pType == ENCODER)
	{
		msg_fileSize = loadFileSize(msg_path);
		if (msg_fileSize > spaceInBmp)
		{
			msg_fileSize = spaceInBmp;
		}
	}
	else
	{
		msg_fileSize = mLength;
	}

	if ((bmp_fileSize + msg_fileSize) > maxMemUse)
	{
		return false;
	}
	return true;
}


void MemoryManager::loadGlobalMemStatusEx()
{
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	maxMemUse = statex.ullTotalPhys;
	maxMemUse *= 2;
	maxMemUse /= 3;		//70% mem load
}