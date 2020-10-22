#pragma once

#include <QtWidgets/QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QBitmap>

#include "ui_Steganography.h"
#include "BMP_Manager.h"




class Steganography : public QWidget
{
    Q_OBJECT

public:
    Steganography(QWidget *parent = Q_NULLPTR);
    BMP_Manager bmpManager;

private:
    Ui::SteganographyClass ui;
    


    bool runEnable[3];  
    void checkRunButton();
private slots:
    void on_loadBmpButton_clicked();
    void on_loadMsgButton_clicked();
    void on_resultPathButton_clicked();
    void on_quitButton_clicked();
    void on_runButton_clicked();

    void on_decoderRadioButton_clicked();
    void on_encoderRadioButton_clicked();
};
