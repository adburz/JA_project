#include "Steganography.h"
#define bmpState 0
#define msgState 1
#define resState 2
#define cppAlgo 1



Steganography::Steganography(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    runEnable[bmpState] = runEnable[msgState] = runEnable[resState] = /*false*/true;
}
void Steganography::checkRunButton()
{
    if (runEnable[bmpState] == true && runEnable[msgState] == true && runEnable[resState] == true)
    {
        ui.runButton->setEnabled(true);
    }
    else
    {
        ui.runButton->setEnabled(false);
    }
}

void Steganography::on_loadBmpButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open image"), "", tr("Image files (*.bmp)"));
    if (!fileName.isEmpty())
    {
        bmpManager.set_bmpPath(fileName.toStdString());
        ui.loadPathTextBox->setText(QString(fileName));

        int result = bmpManager.checkImage();

        ui.maxMsgSize->setText(QString::number(result) + " B");
        if (result == 0)
        {
            runEnable[bmpState] = false;
        }
        else
        {
            runEnable[bmpState] = true;

        }
    }
    checkRunButton();
}

void Steganography::on_loadMsgButton_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Set message"), "", tr("Text files (*.txt)"));
    if (!fileName.isEmpty())
    {
        bmpManager.set_msgPath(fileName.toStdString());
        ui.loadMsgTextBox->setText(QString(fileName));

        //!w zalozeniach dalem, ze tylko pliki .txt w kodowaniu ascii, wiec sie chyba nie musze przejmowac 
        //!innymi formatami.
        runEnable[msgState] = true;
    }
    checkRunButton();
}

void Steganography::on_resultPathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
        "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        bmpManager.set_resPath(dir.toStdString());
        ui.resultPathTextBox->setText(QString(dir));
        runEnable[resState] = true;
    }
    checkRunButton();
}

void Steganography::on_quitButton_clicked()
{
    bmpManager.clearData();
    bmpManager.clearMsg();

    QCoreApplication::exit();
}

void Steganography::on_runButton_clicked()
{
    bool programType = ui.encoderRadioButton->isChecked();
    bool algType = ui.algoCppRadioButton->isChecked();
    short threadCount = ui.CPU_threads->value();
    __int64 result = bmpManager.run(programType, algType, threadCount);
    if (algType == cppAlgo)
    {
        ui.cpp_ticks->setText(QString::number(result));
    }
    else
    {
        ui.asm_ticks->setText(QString::number(result));
    }
}

void Steganography::on_decoderRadioButton_clicked()
{
    runEnable[msgState] = true;
    ui.loadMsgButton->setEnabled(false);
    ui.loadMsgTextBox->setText(QString(""));
    checkRunButton();
}

void Steganography::on_encoderRadioButton_clicked()
{
    runEnable[msgState] = false;
    ui.loadMsgButton->setEnabled(true);
    checkRunButton();
}
