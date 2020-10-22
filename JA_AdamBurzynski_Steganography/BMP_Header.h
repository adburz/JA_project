#pragma once
#include <Windows.h>
#include <wingdi.h>



class BMP_Header
{
public:
	BMP_Header();
	BITMAPFILEHEADER fileHeader;
	//!dodac do specyfikacji ze obsluguje tylko ten drugiego headera


	char* fileInfoHeader = nullptr;
	
	int charArrToInt(char* cArray, int offset);
	
	int getImageSize();
	int getHight();
	int getWidth();
	int getByteCount(char* tmp);
	bool is24bit(char * tmpArr);
	bool isBMP();
	unsigned int getOffset();

	int padSize;

private:
	

};