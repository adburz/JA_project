#include "Steganography.h"
#define BMP_STATE 0
#define MSG_STATE 1
#define RES_STATE 2
#define CPP_ALG 1



Steganography::Steganography(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    runEnable[BMP_STATE] = runEnable[MSG_STATE] = runEnable[RES_STATE] = false;
    ui.suggestedN_threads->setText(QString::number(std::thread::hardware_concurrency()));
}
void Steganography::checkRunButton()
{
    if (runEnable[BMP_STATE] == true && runEnable[MSG_STATE] == true && runEnable[RES_STATE] == true)
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
            runEnable[BMP_STATE] = false;
        }
        else
        {
            runEnable[BMP_STATE] = true;

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
        runEnable[MSG_STATE] = true;
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
        runEnable[RES_STATE] = true;
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
    if (algType == CPP_ALG)
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
    runEnable[MSG_STATE] = true;
    ui.loadMsgButton->setEnabled(false);
    ui.loadMsgTextBox->setText(QString(""));
    checkRunButton();
}

void Steganography::on_encoderRadioButton_clicked()
{
    runEnable[MSG_STATE] = false;
    ui.loadMsgButton->setEnabled(true);
    checkRunButton();
}
