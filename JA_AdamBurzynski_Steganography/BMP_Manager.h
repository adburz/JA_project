#pragma once
#include <Windows.h>
#include <QMessageBox>
#include <string>
#include <vector>
#include <thread>
#include <fstream> 
#include <utility>
#include <functional>
#include "BMP_Header.h"
#include "Memory_Manager.h"
#include "ProcTimer.h"

//typedef int(CALLBACK* cppDecoding)(int begin, int end, char* bmpArray, std::vector<char>& decMessage);
//typedef int(CALLBACK* cppEncoding)(char* bmpArr, int aBegin, std::vector<char> msg, int vBegin, int vEnd);


class BMP_Manager
{
public:
	BMP_Manager()
	{
		threadCount = 1;
	}
	int checkImage();
	
	void loadImage();
	
	int getPadding();

	__int64 run(bool programType, bool algType, short tCount);
	void run_partMode(bool programType, bool algType);
	
	void set_resPath(std::string filePath);
	void set_bmpPath(std::string filePath);
	void set_msgPath(std::string filePath);

	void clearData();
private:
	std::string resPath;
	std::string bmpPath;
	std::string msgPath;

	long accMsgMem;	//accessible message memmory
	unsigned int msgLength; //in bytes/number of chars
	short threadCount;

	long long bmpByteCount;
	char* bmpArray = nullptr;	//bitmapa

	BMP_Header bmpHeader;


	//!Memmory manager
	MemoryManager meMan;
	

	void runEncoder(bool algType);
	void runDecoder(bool algType);

	void saveImage();
	void saveMessage();
	void loadMessage();

	char* encMsg;
	char* decMsg;

	std::vector<char> encMessage;
	std::vector<std::vector<char>> decMessage;

	//----------TIMER----------
	OperationTimer timer;

	//----------EXPLICIT LINKING----------
	//cppDecoding decoding;    // Function pointer
	//cppEncoding encoding;    // Function pointer
	
	//HINSTANCE hDLL;               // Handle to DLL
	//void LoadDLL(bool algType);
};