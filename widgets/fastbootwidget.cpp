/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


#include "fastbootwidget.h"
#include "ui_fastbootwidget.h"

extern QProcess *adbProces;
extern QString sdk;
extern QString adb;
extern QString aapt;
extern QString busybox;
extern QString fastboot;

FastbootWidget::FastbootWidget(QWidget *parent,Phone *phone) :
    QWidget(parent),
    ui(new Ui::FastbootWidget)
{
    ui->setupUi(this);

    this->setLayout(this->ui->layoutFastboot);
    this->phone=phone;

    connect(this->ui->buttonBootIMG, SIGNAL(clicked()), this, SLOT(bootIMG()));
    connect(this->ui->buttonFlashRadio, SIGNAL(clicked()), this, SLOT(flashRadio()));
    connect(this->ui->buttonFlashRecovery, SIGNAL(clicked()), this, SLOT(flashRecovery()));
    connect(this->ui->buttonFlashSPL, SIGNAL(clicked()), this, SLOT(flashSPL()));
}

FastbootWidget::~FastbootWidget()
{
    delete ui;
}

//void FastbootWidget::changeEvent(QEvent *e)
//{
//    QWidget::changeEvent(e);
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
//        break;
//    default:
//        break;
//    }
//}

void FastbootWidget::bootIMG()
{
    QString output;
    QProcess fastboot_proc;
    fastboot_proc.setProcessChannelMode(QProcess::MergedChannels);
    fastboot_proc.start(QString("\"") + fastboot + "\"", QStringList()<<" devices");
    fastboot_proc.waitForFinished();
    output = fastboot_proc.readAll();
    if (output.contains("fastboot"))
    {
        QProcess *process=new QProcess();
        process->setProcessChannelMode(QProcess::MergedChannels);
        QString tmp;
        QString imgFileName = QFileDialog::getOpenFileName(this, tr("Choose img file..."), ".", tr("IMG File ")+"(*.img)");
        if (!imgFileName.isEmpty())
        {
            process->start("\"" + fastboot + "\"", QStringList()<< imgFileName);
            process->waitForFinished(-1);
            process->terminate();
        }
        else
            QMessageBox::warning(this, "Error!", "Operation cancelled!", QMessageBox::Ok);
        delete process;
    }
    else
    {
        this->phone->slotConnectionChanged(FASTBOOT,this->phone->serialNumber);
    }
}

void FastbootWidget::flashSPL()
{
    QString output;
    QProcess fastboot_proc;
    fastboot_proc.setProcessChannelMode(QProcess::MergedChannels);
    fastboot_proc.start("\"" + fastboot + "\"", QStringList()<<" devices");
    fastboot_proc.waitForFinished();
    output = fastboot_proc.readAll();
    if (output.contains("fastboot"))
    {
        QProcess *process=new QProcess();
        process->setProcessChannelMode(QProcess::MergedChannels);
        QString tmp;
        QString imgFileName = QFileDialog::getOpenFileName(this, tr("Choose hboot img file..."), ".", tr("IMG File ")+"(*.img)");
        if (!imgFileName.isEmpty())
        {
            process->start("\"" + fastboot + "\"", QStringList()<<" flash hboot " + imgFileName);
            process->waitForFinished(-1);
            tmp = process->readAll();
            if (tmp.contains("error"))
                QMessageBox::warning(this, tr("Error!"), tmp, QMessageBox::Ok);
            else
                QMessageBox::information(this, tr("Success!"), tmp, QMessageBox::Ok);
            process->terminate();
        }
        else
            QMessageBox::warning(this, tr("Error!"), tr("Operation cancelled!"), QMessageBox::Ok);
        delete process;
    }
    else
    {
        this->phone->slotConnectionChanged(FASTBOOT,this->phone->serialNumber);
    }
}

void FastbootWidget::flashRadio()
{
    QString output;
    QProcess fastboot_proc;
    fastboot_proc.setProcessChannelMode(QProcess::MergedChannels);
    fastboot_proc.start("\"" + fastboot + "\"", QStringList()<<" devices");
    fastboot_proc.waitForFinished();
    output = fastboot_proc.readAll();
    if (output.contains("fastboot"))
    {
        QProcess *process=new QProcess();
        process->setProcessChannelMode(QProcess::MergedChannels);
        QString tmp;
        QString imgFileName = QFileDialog::getOpenFileName(this, tr("Choose radio img file..."), ".", tr("IMG File ")+"(*.img)");
        if (!imgFileName.isEmpty())
        {
            process->start("\"" + fastboot + "\"", QStringList()<<" flash radio " + imgFileName);
            process->waitForFinished(-1);
            tmp = process->readAll();
            if (tmp.contains("error"))
                QMessageBox::warning(this, tr("Error!"), tmp, QMessageBox::Ok);
            else
                QMessageBox::information(this, tr("Success!"), tmp, QMessageBox::Ok);
            process->terminate();
        }
        else
            QMessageBox::warning(this, tr("Error!"), tr("Operation cancelled!"), QMessageBox::Ok);
        delete process;
    }
    else
    {
        this->phone->slotConnectionChanged(FASTBOOT,this->phone->serialNumber);
    }
}

void FastbootWidget::flashRecovery()
{
    QString output;
    QProcess fastboot_proc;
    fastboot_proc.setProcessChannelMode(QProcess::MergedChannels);
    fastboot_proc.start("\"" + fastboot + "\"" , QStringList() <<" devices");
    fastboot_proc.waitForFinished();
    output = fastboot_proc.readAll();
    if (output.contains("fastboot"))
    {
        QProcess *process=new QProcess();
        process->setProcessChannelMode(QProcess::MergedChannels);
        QString tmp;
        QString imgFileName = QFileDialog::getOpenFileName(this, tr("Choose recovery img file..."), ".", tr("IMG File ")+"(*.img)");
        if (!imgFileName.isEmpty())
        {
            process->start("\"" + fastboot + "\"", QStringList()<<" flash recovery " + imgFileName);
            process->waitForFinished(-1);
            tmp = process->readAll();
            if (tmp.contains("error"))
                QMessageBox::warning(this, tr("Error!"), tmp, QMessageBox::Ok);
            else
                QMessageBox::information(this, tr("Success!"), tmp, QMessageBox::Ok);
            process->terminate();
        }
        else
            QMessageBox::warning(this, tr("Error!"), tr("Operation cancelled!"), QMessageBox::Ok);
        delete process;
    }
    else
    {
        this->phone->slotConnectionChanged(FASTBOOT,this->phone->serialNumber);
    }
}
