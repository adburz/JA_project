#pragma once
#include <QMessageBox>
#include <string>
#include <vector>
#include <fstream> //ifstream
#include "BMP_Header.h"


class BMP_Manager
{
public:
	BMP_Manager()
	{
	}
	int checkImage();
	
	void loadImage();
	
	int getPadding();

	void run(bool programType, bool algType, short threadCount);
	
	void set_resPath(std::string filePath);
	void set_bmpPath(std::string filePath);
	void set_msgPath(std::string filePath);

	void clearData();
private:
	std::string resPath;
	std::string bmpPath;
	std::string msgPath;

	long accMsgMem;	//accessible message memmory
	long msgLength;
	BMP_Header bmpHeader;

	void runEncoder(bool algType);
	void runDecoder(bool algType);

	void cppEncoding(char* &bmpArr, std::vector<char> encMessage);
	void cppDecoding(int begin, int end, char* bmpArray, std::vector<char> &decMessage);

	void saveImage();
	void saveMessage();


	char* bmpArray = nullptr;	//bitmapa



	void loadMessage();


	
	std::vector<char> encMessage;
	std::vector<std::vector<char>> decMessage;

};