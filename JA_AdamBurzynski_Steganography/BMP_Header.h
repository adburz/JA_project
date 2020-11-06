#pragma once
#include <Windows.h>
#include <wingdi.h>



class BMP_Header
{
public:
	BMP_Header() {};
	BITMAPFILEHEADER fileHeader;

	char* fileInfoHeader = nullptr;
	
	int charArrToInt(char* cArray, int offset);
	
	void setMsgCharCount(unsigned int mCount);
	unsigned int getMsgCharCount();

	int getImageSize();
	int getHight();
	int getWidth();
	int getByteCount(char* tmp);
	bool is24bit(char * tmpArr);
	bool isBMP();

	int padSize;


};