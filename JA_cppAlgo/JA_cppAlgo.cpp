#include "pch.h"
#include <vector>
#include "JA_cppAlgo.h"

#define BMP_BYTE_COUNT_FOR_ONE_CHAR 24




void decoding(char* bmpArray, char* msg, int mEnd)
{
	short LSB;
	char letter = 0;
	for (int j = 0; j < mEnd; j++)
	{
		for (int i = 1; i <= BMP_BYTE_COUNT_FOR_ONE_CHAR; i += 3)
		{
			LSB = ((unsigned char)bmpArray[i + j * BMP_BYTE_COUNT_FOR_ONE_CHAR]) % 2;
			if (LSB == 1)
			{
				//letter = letter || insertOne;
				letter += 0b10000000;
				//changed = true;
				if (i != 22)
					letter >>= 1;
				letter -= 0b10000000;
			}
			else if (i != 22)//bo przesuwam o 1 raz duzo 
			{
				letter >>= 1;
			}
		}
		*(msg + j) = letter;
		letter = 0;
	}
}



void encoding(char* bmpArray, char* msg, int mEnd)
{
	short LSB;
	unsigned char tmp;
	for (int j = 0; j < mEnd; j++)
	{
		for (int i = 1; i <= BMP_BYTE_COUNT_FOR_ONE_CHAR; i += 3)	//i=1, bo zaczynamy kodowac G (BGR)
		{
			LSB = msg[j] % 2;	// czytamy sobie ostatni bit

			tmp = ((unsigned char)bmpArray[BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i]) % 2;
			if (tmp != LSB)	//jesli mozna pominac zamiane 
			{
				*(bmpArray + (BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i)) =
					(*(bmpArray + (BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i)) + 1) * LSB
					+ (*(bmpArray + (BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i)) - 1) * !LSB;
			}
			tmp = *(msg + j);
			tmp >>= 1;			//przesuwamy bity przygotowujac na nastepny bit do czytania]
			*(msg + j) = tmp;

		}
	}
}