#include "Memory_Manager.h"
#define ENCODER true
void MemoryManager::loadLimits(int offsetPosition, long width)
{
	//current_fileSize = fileSize;
	//current_fileSize -= offsetPosition;

	//current_loaded += offsetPosition;

	////obliczam ile bajtow mozna zarezerwowac dla bitmapy a ile dla wiadomosci.

	//DWORDLONG tmp = maxMemUse / 8;
	//maxBMP_size = maxMemUse - tmp; 
	//maxMsg_size = maxBMP_size / 8;
	//int msgMod = maxMsg_size % 8;
	//maxMsg_size -= msgMod;
	//maxBMP_size = maxMsg_size * 8;

	//usingMem = maxBMP_size + maxMsg_size;


	////todo chyba to tak jednak
	//maxMsg_size = maxMemUse / 25;
	//msgMod = maxMsg_size % 8;
	//maxMsg_size -= msgMod;
	//maxBMP_size = maxMsg_size * 24;
}

std::uintmax_t MemoryManager::loadFileSize(std::string fPath)
{
	std::uintmax_t fileSize = std::filesystem::file_size(fPath);
	return fileSize;
}

bool MemoryManager::isEnoughSpace(std::string bmp_path, std::string msg_path, bool pType, unsigned int mLength)
{
	loadGlobalMemStatusEx();
	bmp_fileSize = loadFileSize(bmp_path);
	if (pType == ENCODER)
	{
		msg_fileSize = loadFileSize(msg_path);
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