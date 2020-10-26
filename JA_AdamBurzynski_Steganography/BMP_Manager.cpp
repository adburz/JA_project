#include "BMP_Manager.h"
#define byteSize 8 //bits
#define encoder true
#define cppAlg true
#define RGBspace 3

#define insertOne	0x80
#define insertZero	0x7F

extern "C" void _stdcall cppEncoding(char* bmpArr, int aBegin, char* msg, int mBegin, int mEnd);
extern "C" void _stdcall cppDecoding(char* bmpArray, int aBegin, char* msg, int mBegin, int mEnd);


//void BMP_Manager::LoadDLL(bool algType)
//{
//	//TODO ----------------------------
//	//if (algType == cppAlg)
//	//{
//	//	this->hDLL = LoadLibraryA("JA_cppAlgo");
//	//	if (this->hDLL != NULL)
//	//	{
//	//		decoding = (cppDecoding)GetProcAddress(hDLL, "cppDecoding");
//	//		encoding = (cppEncoding)GetProcAddress(hDLL, "cppEncoding");
//	//	}
//	//	else
//	//	{
//	//		// handle the error
//	//		FreeLibrary(hDLL);
//	//		// here is place to some expection
//	//	}
//	//}
//	//else
//	//{
//	//	this->hDLL = LoadLibraryA("JA_asmAlgo");
//	//}
//	//TODO ----------------------------
//
//}



int BMP_Manager::checkImage()
{
	clearData();

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

void BMP_Manager::clearMsg()
{
	if (message != nullptr)
	{
		delete[] message;
	}
	message = nullptr;
}

void BMP_Manager::loadMessage()
{
	//!obsluga bledow
	std::uintmax_t msgSize = meMan.loadFileSize(msgPath);
	std::ifstream inMsg = std::ifstream(msgPath, std::ios::binary);
	clearMsg();

	message = new char[msgSize];
	if (inMsg.is_open())
	{
		inMsg.read(message, sizeof(char)*msgSize);
		msgLength = msgSize;
	}
	inMsg.close();
}

void BMP_Manager::saveMessage()
{
	std::ofstream output = std::ofstream(resPath + "/result_message.txt", std::ios::binary);
	output.write(message, sizeof(char) * msgLength);
	output.close();
}

void BMP_Manager::loadImage()
{
	clearData();
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
		int widthBytes = (width * RGBspace);
		for (int i = 0; i < height; i++)
		{
			fileStream.read(&this->bmpArray[i * widthBytes], sizeof(char) * widthBytes);
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
	if (msgLength < threadCount)
	{
		threadCount = msgLength;	//czyli na kazdy watek przypada 1 znak.
	}
	if (algType == cppAlg)
	{
		
		long moduloRest = msgLength % threadCount; //reszta ktora trzeba dolozyc do jednego z watkow
		long threadOffset = msgLength / threadCount;

		int i;
		//TODO LoadDLL(cppAlg);
		for (i = 0; i < threadCount - 1/*bo ostatni thread musi dostac reszte modulo*/; i++)
		{
			//TODO LoadDLL(cppAlg);
			std::thread th(cppEncoding, this->bmpArray, i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace,
				this->message, i * threadOffset, threadOffset);

			threadV.push_back(std::move(th));
			//TODO FreeLibrary(hDLL);
			//cppEncoding(this->bmpArray, 0+i* threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace, encMessage, 0 + i * threadOffset, threadOffset);
		}
		//TODO LoadDLL(cppAlg);
		std::thread th(cppEncoding, this->bmpArray, i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace,
			this->message, i * threadOffset, threadOffset + moduloRest);
		threadV.push_back(std::move(th));
		//TODO FreeLibrary(hDLL);

		timer.start();
		for (int j = 0; j < threadCount /*bo ostatni thread musi dostac reszte modulo*/; j++)
		{
			threadV[j].join();
		}
		timer.stop();
		
		this->bmpHeader.setMsgCharCount(msgLength);
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
	std::vector<std::thread> threadV;
	if (msgLength < threadCount)
	{
		threadCount = msgLength;	//czyli na kazdy watek przypada 1 znak.
	}
	if (algType == cppAlg)
	{
		long moduloRest = msgLength % threadCount; //reszta ktora trzeba dolozyc do jednego z watkow
		long thMsgOffset = msgLength / threadCount;
		int i=0;
		for (i = 0; i < threadCount - 1; i++)
		{
			//std::vector<char> msgVector{};
			//decMessage.push_back(msgVector);
			std::thread th(cppDecoding, bmpArray,i * thMsgOffset/*!msgLength ilosc znakow*/ * 8 *RGBspace, message, i * thMsgOffset, thMsgOffset);

			threadV.push_back(std::move(th));
		}
		//std::vector<char> msgVector{};
		//decMessage.push_back(msgVector);
		std::thread th(cppDecoding, bmpArray, i * thMsgOffset/*!msgLength ilosc znakow*/ * 8 * RGBspace, message, i * thMsgOffset, thMsgOffset + moduloRest);
		
		threadV.push_back(std::move(th));
	
		//TODO przerzucic do osobnej funkcji =====================
		timer.start();
		for (int j = 0; j < threadCount /*bo ostatni thread musi dostac reszte modulo*/; j++)
		{
			threadV[j].join();
		}
		timer.stop();
		//decMessage.push_back(msgVector);
		//TODO ==================================================
	}
	else
	{

	}
}

__int64 BMP_Manager::run(bool programType, bool algType, short tCount)
{
	threadCount = tCount;
	/*if(this->meMan.isEnoughSpace(bmpPath, msgPath))
	{ 
		if (programType == encoder)
		{
			run_partMode_encoder(algType);
		}
		else
		{
			run_partMode_decoder(algType);
		}
	}*/
	if(this->meMan.isEnoughSpace(bmpPath, msgPath, programType, this->msgLength))
	{
		loadImage();
		if (programType == encoder)
		{
			runEncoder(algType);
			saveImage();
		}
		else //decoder
		{
			this->msgLength = this->bmpHeader.getMsgCharCount();
			this->message = new char[this->msgLength];
			runDecoder(algType);
			saveMessage();
		}
		clearData();
		return timer.getCounterTotalTicks();
	}
	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Not enough free memory to run program."),
		QMessageBox::Ok, nullptr);
	mBox.exec();
	return 0;
}

void BMP_Manager::loadHeaders()
{
	//TODO fileStream.read((char*)&this->bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));
	//TODO this->bmpHeader.fileInfoHeader = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
	//TODO fileStream.read(this->bmpHeader.fileInfoHeader, this->bmpHeader.fileHeader.bfOffBits - 14);
}

void BMP_Manager::run_partMode_encoder(bool algType)
{
	//8550831445 
	// /8 = 1 068 853 930,625
	// -mod8  = 1 068 853 928

	//odejmujemy = 7 481 977 517
	//7 481 977 512


	

	clearData();
	//TODO najpierw robie encoding w partMode
	std::ifstream bmp_fileStream = std::ifstream(bmpPath, std::ios::binary | std::ios::in);
	if (bmp_fileStream.is_open())
	{
		int offsetPosition = this->bmpHeader.fileHeader.bfOffBits - 14;
		bmp_fileStream.read((char*)&this->bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));
		this->bmpHeader.fileInfoHeader = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
		bmp_fileStream.read(this->bmpHeader.fileInfoHeader, offsetPosition);


		int padding = this->getPadding();
		char* padTrash = new char[padding];
		long width = bmpHeader.getWidth();
		long height = bmpHeader.getHight();
		long bmpSize = width * height * RGBspace;

		meMan.loadLimits(offsetPosition, width);
		
		long widthBytes = (width * RGBspace);

		int bmp_loopCounter = meMan.maxBMP_size / widthBytes;

		long loaded = 0;
		
		while (loaded < bmpSize)
		{
			
			message = new char[meMan.maxMsg_size];

			for (int j = 0; j < meMan.maxMsg_size; j++)
			{
				
			}
			bmpArray = new char[meMan.maxBMP_size];
			for (int i = 0; i < meMan.maxMsg_size * 24; i++)
			{
				bmp_fileStream.read(&this->bmpArray[i * widthBytes], sizeof(char) * widthBytes);
				bmp_fileStream.read(padTrash, sizeof(char) * padding);
			}



			//TODO operacja na danych.

			clearData();
			loaded += (sizeof(char) * widthBytes);
		}
		
		//TODO zostaje uglyRest do zaladowania
	}

}

void BMP_Manager::run_partMode_decoder(bool algType)
{
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
	{
		delete[]bmpHeader.fileInfoHeader;
	}
	
	
	bmpHeader.fileInfoHeader = nullptr;
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

