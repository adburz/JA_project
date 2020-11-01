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





class BMP_Manager
{
public:
	BMP_Manager()
	{
		threadCount = 1;
		resPath = "C:/Users/Adam/Desktop/projekt";
		bmpPath;
		msgPath = "C:/Users/Adam/Desktop/projekt/wiadomosc.txt";
	}
	int checkImage();
	
	void loadHeaders();
	void loadImage();
	
	int getPadding();

	__int64 run(bool programType, bool algType, short tCount);
	void run_partMode_encoder(bool algType);
	void run_partMode_decoder(bool algType);
	
	void set_resPath(std::string filePath);
	void set_bmpPath(std::string filePath);
	void set_msgPath(std::string filePath);

	void clearMsg();
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
	unsigned int loadMessage();

	char* message = nullptr;


	//----------TIMER----------
	OperationTimer timer;

	//----------EXPLICIT LINKING----------
	typedef void(CALLBACK *DECODING)(char* bmpArray, char* msg, int mEnd);
	typedef void(CALLBACK *ENCODING)(char* bmpArray, char* msg, int mEnd);
	DECODING decoding;    // Function pointer
	ENCODING encoding;    // Function pointer
	
	HINSTANCE hDLL;               // Handle to DLL
	void LoadDLL(bool algType);
};