#include "BMP_Manager.h"
#define byteSize 8 //bits
#define encoder true
#define cppAlg true
#define RGBspace 3

#define insertOne	0x80
#define insertZero	0x7F

extern "C" void _stdcall cppEncoding(char* bmpArr, int aBegin, std::vector<char> msg, int vBegin, int vEnd);
extern "C" void _stdcall cppDecoding(int begin, int end, char* bmpArray, std::vector<char> & decMessage);

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

int BMP_Manager::checkImage()
{
	clearData();
	if (GetFileSize(bmpPath))
	{

	}
	std::ifstream fileStream = std::ifstream(bmpPath, std::ios::binary | std::ios::in);
	if (fileStream.is_open())
	{
		fileStream.read((char*)&this->bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));
		if (bmpHeader.isBMP())
		{
			
			//!tego nie trzeba
			//!sprawdzac kodowanie kolorow
			char* tmp = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
			fileStream.read(tmp, this->bmpHeader.fileHeader.bfOffBits - 14);

			if (bmpHeader.is24bit(tmp))
			{
				accMsgMem = this->bmpHeader.getByteCount(tmp) / byteSize;
				accMsgMem /= RGBspace;
				fileStream.close();
				return accMsgMem;
			}
			else
			{
				QMessageBox mBox(QMessageBox::Warning, QString("Warning!"), QString("File isn't coded 24B/pixel."),
					QMessageBox::Ok, nullptr);
				mBox.exec();
				fileStream.close();
			}
		}
		else
		{
			QMessageBox mBox(QMessageBox::Warning, QString("Warning!"), QString("File isn't .bmp."),
				QMessageBox::Ok, nullptr);
			mBox.exec();
			fileStream.close();
		}
	}
	else
	{
		QMessageBox mBox(QMessageBox::Warning, QString("Warning!"), QString("Can't open .bmp file.\nProbably the file path is invalid."),
			QMessageBox::Ok, nullptr);
		mBox.exec();
	}
	clearData();
	fileStream.close();
	return 0;
}

//void BMP_Manager::cppEncoding(char*& bmpArr, std::vector<char> msg) //!msgLength
//{
//	short r;
//	for (int j = 0; j < msg.size(); j++)
//	{
//		for (int i = 1; i <= 8 * RGBspace; i+= RGBspace)	//i=1, bo zaczynamy kodowac G (BGR)
//		{
//			r = msg[j] % 2;	// czytamy sobie ostatni bit
//
//			unsigned char tmp = ((unsigned char)bmpArr[8 * j + i]) % 2;
//			if (tmp != r)	//jesli mozna pominac zamiane 
//			{
//				if (r == 1)
//				{							//wiadomo ze na koncu jest 0, bo nie weszlo w ifa 54
//					bmpArr[8 * j + i] += 1;	//dodajemy 1 na koniec
//				}
//				else						//przypadek gdzie na koncu jest 1, a r=0
//				{
//					bmpArr[8 * j + i] -= 1;	//!porpawic to
//				}
//			}
//			msg[j] >>= 1;			//przesuwamy bity przygotowujac na nastepny bit do czytania
//		}
//	}
//}
//
//void BMP_Manager::cppDecoding(int begin, int end, char* bmpArray, std::vector<char>& decMessage)
//{
//	short r;
//	char letter = 0;
//	bool bylo = false;
//	for (int j = begin; j < end; ++j)
//	{
//		for (int i = 1; i <= 8* RGBspace; i+= RGBspace)
//		{
//			r = ((unsigned char)bmpArray[i + j * 8]) % 2;
//
//			if (r == 1)
//			{
//				//letter = letter || insertOne;
//				letter += 0b10000000;
//				bylo = true;
//			}
//			if (i != 22)	//bo przesuwam o 1 raz duzo 
//				letter >>= 1;
//
//			if (bylo)
//			{
//				letter -= 0b10000000;
//			}
//			bylo = false;
//		}
//		decMessage.push_back(letter);
//		letter = 0;
//	}
//
//}

void BMP_Manager::loadMessage()
{
	//!obsluga bledow
	std::ifstream inMsg = std::ifstream(msgPath, std::ios::binary);
	if (inMsg.is_open())
	{
		encMessage.clear();
		int i = 0;
		while (!inMsg.eof() && i < accMsgMem)
		{
			encMessage.push_back(inMsg.get());
			i++;
		}
		msgLength = i;
	}
}
void BMP_Manager::saveMessage()
{
	std::ofstream output = std::ofstream(resPath + "/result_message.txt", std::ios::binary);
	char* oc = decMessage[0].data();
	output.write(decMessage[0].data(), decMessage[0].size());
	output.close();
}

void BMP_Manager::loadImage()
{
	clearData();
	//!czyszczenie buforow funkcja 
	//!----- wrzucic powtarzana czesc do osobnej f
	std::ifstream fileStream = std::ifstream(bmpPath, std::ios::binary | std::ios::in);
	if (fileStream.is_open())
	{
		fileStream.read((char*)&this->bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));

		this->bmpHeader.fileInfoHeader = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
		fileStream.read(this->bmpHeader.fileInfoHeader, this->bmpHeader.fileHeader.bfOffBits - 14);
		//!poprawic wprowadzanie danych
		int padding = this->getPadding();
		int width = bmpHeader.getWidth();
		int height = bmpHeader.getHight();
		char* padTrash = new char[padding];

		bmpByteCount = width * height * RGBspace;
		bmpArray = new char[bmpByteCount];

		for (int i = 0; i < height; i++)
		{
			fileStream.read(&this->bmpArray[i * width * RGBspace], sizeof(char) * width * 3);
			fileStream.read(padTrash, sizeof(char) * padding);
		}
		//this->bmpArray = new char[bmpHeader.getByteCount()];
		//fileStream.read(this->bmpArray, sizeof(char) * bmpHeader.getByteCount());
	}
	fileStream.close();
}
void BMP_Manager::saveImage()
{
	//!pamietac o padingu
	std::ofstream output = std::ofstream(resPath + "/result_bitmap.bmp", std::ios::binary);
	if (output.is_open())
	{
		output.write((char*)&bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));
		output.write(bmpHeader.fileInfoHeader, this->bmpHeader.fileHeader.bfOffBits - 14);

		int width = bmpHeader.getWidth(), height = bmpHeader.getHight(), padding = bmpHeader.padSize;
		char* padTrash = new char[padding] { 0 };
		for (int i = 0; i < height; i++)
		{
			output.write(&bmpArray[i * width * 3], sizeof(char) * width * 3);
			output.write(padTrash, sizeof(char) * padding);
		}
		output.close();
	}
}

void BMP_Manager::runEncoder(bool algType)
{
	std::vector<std::thread> threadV;
	loadMessage();
	if (algType == cppAlg)
	{
		if (msgLength < threadCount)
		{
			threadCount = msgLength;	//czyli na kazdy watek przypada 1 znak.
		}
		long long moduloRest = msgLength % threadCount; //reszta ktora trzeba dolozyc do jednego z watkow
		long long threadOffset = msgLength / threadCount;


		//!dodac zapisywanie ilosc znakow
		//!do funkcji powinny byc przekazywane dane o odpowiedniej dlugosci, czyli 
		//! dlugosc msg * 8 = ilosc charow w bmpArray, bo na kazdy znak przypada 8 bajtow
		int i;

		for (i = 0; i < threadCount - 1/*bo ostatni thread musi dostac reszte modulo*/; i++)
		{
			std::thread th(cppEncoding, this->bmpArray, 0 + i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace,
				encMessage, 0 + i * threadOffset, threadOffset);

			threadV.push_back(std::move(th));

			//cppEncoding(this->bmpArray, 0+i* threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace, encMessage, 0 + i * threadOffset, threadOffset);
		}
		std::thread th(cppEncoding, this->bmpArray, moduloRest + i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace,
			encMessage, moduloRest + i * threadOffset, threadOffset);
		threadV.push_back(std::move(th));

		timer.start();
		for (int j = 0; j < threadCount /*bo ostatni thread musi dostac reszte modulo*/; j++)
		{
			threadV[j].join();
		}
		timer.stop();
	}
	else //asm algo
	{

	}
	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Encoded successfully."),
		QMessageBox::Ok, nullptr);
	mBox.exec();
}

void BMP_Manager::runDecoder(bool algType)
{
	//!
	decMessage = { {} };
	if (algType == cppAlg)
	{
		for (int i = 0; i < threadCount; i++)
		{
			cppDecoding(0, 7/*!msgLength ilosc znakow*/, bmpArray, decMessage[0]);


		}
	}
	else
	{

	}
}

__int64 BMP_Manager::run(bool programType, bool algType, short tCount)
{
	loadImage();
	threadCount = tCount;
	if (programType == encoder)
	{
		runEncoder(algType);
		saveImage();
	}
	else //decoder
	{
		runDecoder(algType);
		saveMessage();
	}

	return timer.getCounterTotalTicks();
}

void BMP_Manager::set_resPath(std::string filePath)
{
	resPath = filePath;
}

void BMP_Manager::set_bmpPath(std::string filePath)
{
	bmpPath = filePath;

}

void BMP_Manager::set_msgPath(std::string filePath)
{
	msgPath = filePath;
}

void BMP_Manager::clearData()
{
	if (bmpHeader.fileInfoHeader != nullptr)
		delete[]bmpHeader.fileInfoHeader;
}

int BMP_Manager::getPadding()
{
	int pad = 0, width = bmpHeader.getWidth();
	if ((width * 3) % 4 != 0)
	{
		pad = 4 - (width * 3) % 4;
	}
	bmpHeader.padSize = pad;
	return pad;
}

