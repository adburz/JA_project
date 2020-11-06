#include "BMP_Manager.h"
#define BYTE_SIZE 8 //bits
#define ENCODER true
#define CPP_ALG true
#define BMP_BYTE_COUNT_FOR_ONE_CHAR 24
#define BGR_SPACE_BYTE_COUNT 3



bool BMP_Manager::LoadDLL(bool algType)
{
	if (algType == CPP_ALG)
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
			return false;
		}
		return true;
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
			char* tmp = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
			fileStream.read(tmp, this->bmpHeader.fileHeader.bfOffBits - 14);

			if (bmpHeader.is24bit(tmp))
			{
				accMsgMem = this->bmpHeader.getByteCount(tmp) / BYTE_SIZE;
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
			QMessageBox mBox(QMessageBox::Warning, QString("Warning!"), QString("Wrong format of file."),
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

bool BMP_Manager::msgIsAscii()
{
	for (int i = 0; i < msgLength; i++)
	{
		if (message[i] > 0b011111111 || message[i] < 0b00000000)	//sprawdzam czy kazdy bajt wiadomosci miesci sie w zakresie 0-127
		{
			return false;
		}
	}
	return true;
}

unsigned int BMP_Manager::loadMessage()
{
	std::uintmax_t msgSize = meMan.loadFileSize(msgPath);
	clearMsg();
	if (msgSize > accMsgMem)
	{
		msgSize = accMsgMem;
	}

	std::ifstream inMsg = std::ifstream(msgPath, std::ios::binary);
	message = new char[msgSize];
	if (inMsg.is_open())
	{
		inMsg.read(message, sizeof(char) * msgSize);
		msgLength = msgSize;
	}
	inMsg.close();
	if (msgIsAscii())
	{
		return msgLength;
	}
	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Message is wrong."),
		QMessageBox::Ok, nullptr);
	mBox.exec();
	return 0;
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
	std::ifstream fileStream = std::ifstream(bmpPath, std::ios::binary | std::ios::in);
	if (fileStream.is_open())
	{
		fileStream.read((char*)&this->bmpHeader.fileHeader, sizeof(BITMAPFILEHEADER));

		this->bmpHeader.fileInfoHeader = new char[this->bmpHeader.fileHeader.bfOffBits - 14];
		fileStream.read(this->bmpHeader.fileInfoHeader, this->bmpHeader.fileHeader.bfOffBits - 14);
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
	}
	fileStream.close();
}
void BMP_Manager::saveImage()
{
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
		if (LoadDLL(algType))
		{
			timer.start();
			for (i = 0; i < threadCount - 1/*bo ostatni thread musi dostac reszte modulo*/; i++)
			{
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

			for (int j = 0; j < threadCount /*bo ostatni thread musi dostac reszte modulo*/; j++)
			{
				threadV[j].join();
			}
			timer.stop();

			FreeLibrary(hDLL);
			this->bmpHeader.setMsgCharCount(msgLength);
			QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Encoded successfully."),
				QMessageBox::Ok, nullptr);
			mBox.exec();
		}
		else
		{
			QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Loading dll failed. "),
				QMessageBox::Ok, nullptr);
			mBox.exec();
		}
		
	}
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

		if (LoadDLL(algType))
		{
			timer.start();
			for (i = 0; i < threadCount - 1; i++)
			{
				std::thread th(decoding, (this->bmpArray + (i * thMsgOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)), (message + (i * thMsgOffset)), thMsgOffset);
				threadV.push_back(std::move(th));
			}

			std::thread th(decoding, (this->bmpArray + (i * thMsgOffset * BMP_BYTE_COUNT_FOR_ONE_CHAR)), (message + (i * thMsgOffset)), thMsgOffset + moduloRest);
			threadV.push_back(std::move(th));

			for (int j = 0; j < threadCount/*threadV.size()*/ /*bo ostatni thread musi dostac reszte modulo*/; j++)
			{
				threadV[j].join();
			}
			timer.stop();
			FreeLibrary(hDLL);
			QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Decoded successfully."),
				QMessageBox::Ok, nullptr);
			mBox.exec();
		}
		else
		{
			QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Loading dll failed. "),
				QMessageBox::Ok, nullptr);
			mBox.exec();
		}
	}
}

__int64 BMP_Manager::run(bool programType, bool algType, short tCount)
{
	threadCount = tCount;
	if (this->meMan.isEnoughSpace(bmpPath, msgPath, programType, this->msgLength, this->accMsgMem)
		&& checkImage() != 0)
	{
			loadImage();
			if (programType == ENCODER)
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

		return timer.getCounterTotalTicks();
	}

	QMessageBox mBox(QMessageBox::Warning, QString("Information."), QString("Not enough free memory to run program \nor file path does not exist."),
		QMessageBox::Ok, nullptr);
	mBox.exec();
	return 0;
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

