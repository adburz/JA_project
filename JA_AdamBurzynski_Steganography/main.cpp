#include "Steganography.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Steganography w;
    w.show();
    return a.exec();
}
