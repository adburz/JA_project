#include "BMP_Header.h"
#define BMPtype 0x4D42
#define headerOffset 14
#define wantedBPerPixel 24
#define biWidth 4
#define biHeight 8 
#define RGB 3


void BMP_Header::setMsgCharCount(unsigned int mCount)
{
	short rest = mCount % 2;
	unsigned int first = mCount / 2;
	
	unsigned int second = mCount / 2;
	second += rest;

	fileHeader.bfReserved1 = first;
	fileHeader.bfReserved2 = second;

}
unsigned int BMP_Header::getMsgCharCount()
{
	unsigned int first = fileHeader.bfReserved1 ;
	unsigned int second = fileHeader.bfReserved2 ;

	return first + second;
}

int BMP_Header::charArrToInt(char* cArray, int offset)
{
	int result;
	memcpy(&result, &cArray[offset], sizeof(int));

	return result;
}

int BMP_Header::getWidth()
{
	int width;
	memcpy(&width, &fileInfoHeader[biWidth], sizeof(char) * 4);
	return width;
}
int BMP_Header::getHight()
{
	int height;
	memcpy(&height, &fileInfoHeader[biHeight], sizeof(char) * 4);
	return height;
}

int BMP_Header::getImageSize()
{
	int width, height;

	memcpy(&width, &fileInfoHeader[biWidth], sizeof(char) * 4);
	memcpy(&height, &fileInfoHeader[biHeight], sizeof(char) * 4);

	return (width * height);
}

int BMP_Header::getByteCount(char *tmp)
{
	int width=0, height=0;
	
	memcpy(&width, &tmp[biWidth], sizeof(char) *4);
	memcpy(&height, &tmp[biHeight], sizeof(char) * 4);

	return (width * height * RGB);
}

bool BMP_Header::is24bit(char* tmpArr)
{
	int tmp = tmpArr[28 - headerOffset] + tmpArr[29 - headerOffset];//28 and 29 are bytes in array which informs about how many bytes per pixel
	if (tmp == wantedBPerPixel)
		return true;
	return false;
}


bool BMP_Header::isBMP()
{
	if (fileHeader.bfType == BMPtype)
	{
		return true;
	}
	return false;
}
