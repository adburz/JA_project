#include "pch.h"
#include <vector>
#include "JA_cppAlgo.h"

#define RGBspace 3

void cppDecoding(int begin, int end, char* bmpArray, std::vector<char>& decMessage)
{
	short r;
	char letter = 0;
	bool bylo = false;
	for (int j = begin; j < end; ++j)
	{
		for (int i = 1; i <= 8 * RGBspace; i += RGBspace)
		{
			r = ((unsigned char)bmpArray[i + j * 8 * RGBspace]) % 2;

			if (r == 1)
			{
				//letter = letter || insertOne;
				letter += 0b10000000;
				bylo = true;
			}
			if (i != 22)	//bo przesuwam o 1 raz duzo 
				letter >>= 1;

			if (bylo)
			{
				letter -= 0b10000000;
			}
			bylo = false;
		}
		decMessage.push_back(letter);
		letter = 0;
	}

}

void cppEncoding(char* bmpArr, int aBegin, std::vector<char> msg, int vBegin, int vEnd) //!msgLength
{
	short r;
	for (int j = 0; j < vEnd; j++)
	{
		for (int i = 1; i <= 8 * RGBspace; i += RGBspace)	//i=1, bo zaczynamy kodowac G (BGR)
		{
			r = msg[vBegin + j] % 2;	// czytamy sobie ostatni bit

			int x = 8 * j * RGBspace + i + aBegin;
			unsigned char tmp = ((unsigned char)bmpArr[8 * j *RGBspace + i + aBegin]) % 2;
			if (tmp != r)	//jesli mozna pominac zamiane 
			{
				if (r == 1)
				{							//wiadomo ze na koncu jest 0, bo nie weszlo w ifa 54
					*(bmpArr + (8 * j * RGBspace + i + aBegin)) += 1;	//dodajemy 1 na koniec
				}
				else						//przypadek gdzie na koncu jest 1, a r=0
				{
					*(bmpArr + (8 * j * RGBspace + i + aBegin)) -= 1;	//!porpawic to
				}
			}
			msg[vBegin + j] >>= 1;			//przesuwamy bity przygotowujac na nastepny bit do czytania
		}
	}
}