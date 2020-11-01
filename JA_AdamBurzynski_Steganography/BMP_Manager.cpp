#include "BMP_Manager.h"
#define byteSize 8 //bits
#define encoder true
#define cppAlg true
#define BMP_BYTE_COUNT_FOR_ONE_CHAR 24
#define BGR_SPACE_BYTE_COUNT 3

#define insertOne	0x80
#define insertZero	0x7F


void BMP_Manager::LoadDLL(bool algType)
{
	if (algType == cppAlg)
	{
		this->hDLL = LoadLibraryA("JA_cppAlgo");
	}
	else
	{
		this->hDLL = LoadLibraryA("JA_asmAlgo");
	}

	if (this->hDLL != NULL)
	{
		encoding = (ENCODING)GetProcAddress(hDLL, "encoding");
		decoding = (DECODING)GetProcAddress(hDLL, "decoding");

		if (!decoding || !encoding)
		{
			FreeLibrary(hDLL);
		}

	}
}


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
				accMsgMem /= BGR_SPACE_BYTE_COUNT;
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

void BMP_Manager::clearMsg()
{
	if (message != nullptr)
	{
		delete[] message;
	}
	message = nullptr;
}

unsigned int BMP_Manager::loadMessage()
{
	//!obsluga bledow
	std::uintmax_t msgSize = meMan.loadFileSize(msgPath);
	std::ifstream inMsg = std::ifstream(msgPath, std::ios::binary);
	clearMsg();
	if (msgSize > accMsgMem)
	{
		msgSize = accMsgMem;
	}

	message = new char[msgSize];
	if (inMsg.is_open())
	{
		inMsg.read(message, sizeof(char) * msgSize);
		msgLength = msgSize;
	}
	inMsg.close();

	return msgSize;
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

		bmpByteCount = width * height * BGR_SPACE_BYTE_COUNT;
		bmpArray = new char[bmpByteCount];
		int widthBytes = (width * BGR_SPACE_BYTE_COUNT);

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
	if (loadMessage())
	{
		if (msgLength < threadCount)
		{
			threadCount = msgLength;	//czyli na kazdy watek przypada 1 znak.
		}

		long moduloRest = msgLength % threadCount; //reszta ktora trzeba dolozyc do jednego z watkow
		long threadOffset = msgLength / threadCount;

		int i;
		LoadDLL(algType);
		//encoding(this->bmpArray, i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace, this->message, i * threadOffset, threadOffset);
		//encoding(this->bmpArray, 1, this->message, 2, 3);
		 //TODO encoding((this->bmpArray + (i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace)),(this->message + (i * threadOffset)), 3);
		for (i = 0; i < threadCount - 1/*bo ostatni thread musi dostac reszte modulo*/; i++)
		{
			//std::thread th(encoding, this->bmpArray, i * threadOffset * 8/*!bo kazdy B wiad = 8B image*/ * RGBspace,
			//	this->message, i * threadOffset, threadOffset);

			std::thread th(encoding,
				(this->bmpArray + (i * threadOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)),
				(this->message + (i * threadOffset)),
				threadOffset);
			threadV.push_back(std::move(th));
		}

		std::thread th(encoding,
			(this->bmpArray + (i * threadOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)),
			(this->message + (i * threadOffset)),
			threadOffset + moduloRest);
		threadV.push_back(std::move(th));

		timer.start();
		for (int j = 0; j < threadCount /*bo ostatni thread musi dostac reszte modulo*/; j++)
		{
			threadV[j].join();
		}
		timer.stop();
		FreeLibrary(hDLL);

		this->bmpHeader.setMsgCharCount(msgLength);

	}
	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Encoded successfully."),
		QMessageBox::Ok, nullptr);
	mBox.exec();
}

void BMP_Manager::runDecoder(bool algType)
{
	if (msgLength != 0)
	{
		std::vector<std::thread> threadV;
		if (msgLength < threadCount)
		{
			threadCount = msgLength;	//czyli na kazdy watek przypada 1 znak.
		}
		int i = 0;
		long moduloRest = msgLength % threadCount; //reszta ktora trzeba dolozyc do jednego z watkow
		long thMsgOffset = msgLength / threadCount;

		LoadDLL(algType);
		//decoding(bmpArray, i * thMsgOffset/*!msgLength ilosc znakow*/, message, i * thMsgOffset, thMsgOffset);


		for (i = 0; i < threadCount - 1; i++)
		{
			std::thread th(decoding, (this->bmpArray + (i * thMsgOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)), (message + (i * thMsgOffset)), thMsgOffset);
			threadV.push_back(std::move(th));
		}

		std::thread th(decoding, (this->bmpArray + (i * thMsgOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)), (message + (i * thMsgOffset)), thMsgOffset + moduloRest);
		threadV.push_back(std::move(th));

		timer.start();
		for (int j = 0; j < threadCount/*threadV.size()*/ /*bo ostatni thread musi dostac reszte modulo*/; j++)
		{
			threadV[j].join();
		}
		timer.stop();
		FreeLibrary(hDLL);
	}
}

__int64 BMP_Manager::run(bool programType, bool algType, short tCount)
{
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
	if (programType == encoder)
	{
		bmpPath = "C:/Users/Adam/Desktop/projekt/sample.bmp";
	}
	else
	{
		bmpPath = "C:/Users/Adam/Desktop/projekt/result_bitmap.bmp";
	}

	threadCount = tCount;
	if (this->meMan.isEnoughSpace(bmpPath, msgPath, programType, this->msgLength))
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
	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Not enough free memory to run program \nor file path does not exist."),
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
		long bmpSize = width * height * BGR_SPACE_BYTE_COUNT;

		meMan.loadLimits(offsetPosition, width);

		long widthBytes = (width * BGR_SPACE_BYTE_COUNT);

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

