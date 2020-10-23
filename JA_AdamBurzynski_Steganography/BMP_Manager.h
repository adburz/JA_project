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
#include "ProcTimer.h"


class BMP_Manager
{
public:
	BMP_Manager()
	{
	}
	int checkImage();
	
	void loadImage();
	
	int getPadding();

	__int64 run(bool programType, bool algType, short tCount);
	
	void set_resPath(std::string filePath);
	void set_bmpPath(std::string filePath);
	void set_msgPath(std::string filePath);

	void clearData();
private:
	std::string resPath;
	std::string bmpPath;
	std::string msgPath;

	long accMsgMem;	//accessible message memmory
	long long msgLength; //in bytes/number of chars
	short threadCount;

	long long bmpByteCount;
	char* bmpArray = nullptr;	//bitmapa

	BMP_Header bmpHeader;

	void runEncoder(bool algType);
	void runDecoder(bool algType);

	//void cppEncoding(char* &bmpArr, std::vector<char> encMessage);
	//void cppDecoding(int begin, int end, char* bmpArray, std::vector<char> &decMessage);

	void saveImage();
	void saveMessage();
	void loadMessage();

	std::vector<char> encMessage;
	std::vector<std::vector<char>> decMessage;

	//----------TIMER----------
	OperationTimer timer;

	
	

};