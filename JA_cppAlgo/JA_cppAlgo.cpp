#include "pch.h"
#include <vector>
#include "JA_cppAlgo.h"

#define BMP_BYTE_COUNT_FOR_ONE_CHAR 24
#define LAST_ITERATION_I_VALUE 22




void decoding(char* bmpArray, char* msg, int mEnd)
{
	short LSB;
	unsigned char letter = 0;
	for (int j = 0; j < mEnd; j++)
	{
		for (int i = 1; i <= BMP_BYTE_COUNT_FOR_ONE_CHAR; i += 3)
		{
			LSB = ((unsigned char)bmpArray[i + j * BMP_BYTE_COUNT_FOR_ONE_CHAR]) % 2;
			letter &= 0b01111111;	//zerujemy najbardziej znaczacy bit
			letter |= (0b10000000 * LSB);	//podmieniamy go na porzadana wartosc


			if (i != LAST_ITERATION_I_VALUE)	//aby odkodowac 8 bitow, potrzeba 7 przesuniec
			{
				letter >>= 1;
			}
		}
		*(msg + j) = letter;
	}
}



void encoding(char* bmpArray, char* msg, int mEnd)
{
	short LSB;
	for (int j = 0; j < mEnd; j++)
	{
		for (int i = 1; i <= BMP_BYTE_COUNT_FOR_ONE_CHAR; i += 3)	//i=1, bo zaczynamy kodowac G (BGR)
		{
			LSB = msg[j] % 2;	// czytamy ostatni bit

			*(bmpArray + (BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i)) = (unsigned char)bmpArray[BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i] & 0b11111110;	//zerujemy LSB
			*(bmpArray + (BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i)) = (unsigned char)bmpArray[BMP_BYTE_COUNT_FOR_ONE_CHAR * j + i] | LSB;			//ustawiamy porzadana wartosc

			*(msg + j) = *(msg + j) >> 1;	//przesuwamy bity o 1 w prawo, aby przy nastepnym przejsciu petli pobrac czyste wartosci.
		}
	}
}