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


#include "dialogkopiuj.h"
#include "ui_dialogkopiuj.h"
#include <QSettings>
#include <QTextCodec>
#include <QStringList>

extern QString sdk;
extern QString adb;
extern QString aapt;
extern QProcess *adbProces;
extern QString busybox;
extern QString fastboot;

dialogKopiuj::dialogKopiuj(QWidget *parent, QList<File> *fileList,  int mode, QString sourcePath, QString targetPath) :
        QDialog(parent),
        ui(new Ui::dialogKopiuj)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->sourcePath = sourcePath;
    this->targetPath = targetPath;
    this->mode = mode;

    this->filesCopiedSize = 0;

    if (this->sourcePath.at(this->sourcePath.length() - 1) != '/')
        this->sourcePath.append("/");
    if (this->targetPath.at(this->targetPath.length() - 1) != '/')
        this->targetPath.append("/");

    this->opFinished = false;
    int max = 0;
    for (int i=0; i < fileList->size(); i++)
    {
        max += fileList->at(i).fileSize.toInt();
    }
    this->ui->progressTotal->setMaximum(max);

    this->remains = fileList->size() + 1;

    this->threadProgress = new ThreadProgress;
    this->threadProgress->mode = mode;

    this->threadCopy = new ThreadCopy;
    this->threadCopy->mode = mode;
    this->threadCopy->sourcePath = this->sourcePath;
    this->threadCopy->targetPath = this->targetPath;
    this->threadCopy->fileList = fileList;
    this->threadCopy->start();

    connect(this->threadCopy, SIGNAL(nextFile(QString, QString, QString, int, int)), this, SLOT(nextFile(QString, QString, QString, int, int)));
    connect(this->threadCopy, SIGNAL(copied()), this, SLOT(copied()));
    connect(this->threadCopy, SIGNAL(isRunning()), this, SLOT(running()));
    connect(this->ui->buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
    //my new OK button
    connect(this->ui->buttonCopyOK, SIGNAL(clicked()), this, SLOT(close()));
//    connect(this->threadProgress, SIGNAL(progressValue(int)), this->ui->progressFile, SLOT(setValue(int)));
    connect(this->threadProgress, SIGNAL(progressValue(int)), this, SLOT(setProgressValue(int)));

    this->setFixedHeight(180);
    this->setLayout(ui->gridLayout);
}

dialogKopiuj::dialogKopiuj(QWidget *parent, QList<App> *appList, int mode, QString targetPath) :
        QDialog(parent),
        ui(new Ui::dialogKopiuj)
{
    ui->setupUi(this);
    this->targetPath = targetPath;
    this->mode = mode;

    this->filesCopiedSize = 0;
    this->remains = appList->size() + 1;
    if (this->targetPath.at(this->targetPath.length() - 1) != '/')
        this->targetPath.append("/");

    this->opFinished = false;
    int max = 0;
    for (int i=0; i < appList->size(); i++)
    {
        max += appList->at(i).appSize.toInt();
    }
    this->ui->progressTotal->setMaximum(max);

    this->threadProgress = new ThreadProgress;
    this->threadProgress->mode = mode;

    this->threadCopy = new ThreadCopy;
    this->threadCopy->mode = mode;
    this->threadCopy->targetPath = this->targetPath;
    this->threadCopy->appList = appList;
    this->threadCopy->start();

    connect(this->threadCopy, SIGNAL(nextFile(QString, QString, QString, int, int)), this, SLOT(nextFile(QString, QString, QString, int, int)));
    connect(this->threadCopy, SIGNAL(copied()), this, SLOT(copied()));
    connect(this->threadCopy, SIGNAL(isRunning()), this, SLOT(running()));
    connect(this->ui->buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
    //my new OK button
    connect(this->ui->buttonCopyOK, SIGNAL(clicked()), this, SLOT(close()));
//    connect(this->threadProgress, SIGNAL(progressValue(int)), this->ui->progressFile, SLOT(setValue(int)));
    connect(this->threadProgress, SIGNAL(progressValue(int)), this, SLOT(setProgressValue(int)));

    this->setFixedHeight(180);
    this->setLayout(ui->gridLayout);
}

dialogKopiuj::~dialogKopiuj()
{
    if (this->threadProgress != NULL)
        delete this->threadProgress;
    if (this->threadCopy != NULL)
        delete this->threadCopy;
    delete ui;
}

void dialogKopiuj::closeEvent(QCloseEvent *event)
{
    if (!this->opFinished) //copy in progress
    {
        if (QMessageBox::question(this, tr("Close?"), tr("Are you sure??"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) // No pressed
        {
            event->ignore(); //copy continues
        }
        else // Yes pressed
        {
            // Abort copy, dialog closed
            this->setProgressValue(NULL);
            this->threadCopy->proces->kill();
            this->threadCopy->terminate();
            this->threadCopy = NULL;
            delete this->threadCopy;
            this->threadProgress->terminate();
            this->threadProgress = NULL;
            delete this->threadProgress;
            event->accept();
            emit copyCanceled();
        }
    }
    else // copy finished
    {
        this->accept(); // dialog closed
    }
}

void dialogKopiuj::closeAfterFinished()
{
    this->ui->checkBox->setChecked(true);
}

void dialogKopiuj::copied()
{
    this->ui->buttonCancel->setDisabled(true);
    this->ui->buttonCopyOK->setDisabled(false);
    this->opFinished = true;
    if (this->ui->checkBox->isChecked())
        this->close();
}

void dialogKopiuj::running()
{
    //this->ui->buttonCancel->setDisabled(false);
    this->ui->buttonCopyOK->setDisabled(true);
}

void dialogKopiuj::nextFile(QString fileName, QString sourcePath, QString targetPath, int fileSize, int counter)
{
    qDebug()<<"Copy,nextFile() - START";
    QString pathFrom, pathTo;
    pathFrom = sourcePath;
    pathTo = targetPath;

    if (pathFrom.length()>50)
        pathFrom = "..." + pathFrom.right(47);
    if (pathTo.length()>50)
        pathTo = "..." + pathTo.right(47);
    this->ui->labelFile->setText(fileName);
    this->ui->labelFrom->setText(pathFrom);
    this->ui->labelTo->setText(pathTo);
    this->ui->progressFile->setMaximum(fileSize);
    qDebug()<<"Copy,nextFile() - UI setted";
//    this->ui->progressBar_2->setValue(counter);
//    emit this->progressValue(counter, this->ui->progressBar_2->maximum());

    if (this->threadProgress->isRunning())
        this->threadProgress->terminate();
    this->threadProgress->maxSize = fileSize;
    this->filesCopiedSize += fileSize;
    this->setProgressValue(0);
    this->remains--;
    this->threadProgress->filePath = targetPath + fileName;
    this->threadProgress->start();
    qDebug()<<"Copy,nextFile() - threadProgress started";
}

void dialogKopiuj::setProgressValue(int value)
{
    qDebug()<<"Copy, setProgressValue - START: "<<value<<"/"<<this->ui->progressFile->maximum();
    int fileSize = this->ui->progressFile->maximum();
    this->ui->progressFile->setValue(value);
    this->ui->progressTotal->setValue(this->filesCopiedSize - fileSize + value);
    int sizeRemain;
    sizeRemain = this->ui->progressTotal->maximum() - (this->filesCopiedSize - fileSize + value);
    this->ui->labelRemain->setText(QString::number(this->remains) + " ( " + this->humanReadableSize(QString::number(sizeRemain)) + " )");
    if (!this->opFinished)
        emit this->progressValue(this->filesCopiedSize - fileSize + value, this->ui->progressTotal->maximum());
    qDebug()<<"Copy, setProgressValue - END: "<<value<<"/"<<this->ui->progressFile->maximum();
}

void ThreadCopy::run()
{
    QString fileName, command, output, sourceDir, targetDir;
    int fileSize, counter = 0;
    File file;
    App app;
    emit this->isRunning();
    // Ui::dialogKopiuj ui;
  //  Ui::dialogKopiuj()->buttonCopyOK->setDisabled(true);
//    QProcess *proces;

    QTextCodec *codec = QTextCodec::codecForLocale();

    if (this->mode == dialogKopiuj::PhoneToComputer)
    {
        while (this->fileList->size() > 0)
        {
            QDir dir;
            proces = new QProcess;
            proces->setProcessChannelMode(QProcess::MergedChannels);
            counter++;
            file = this->fileList->takeFirst();
            fileName = file.filePath;
            fileName.remove(this->sourcePath);
            fileSize = file.fileSize.toInt();
            sourceDir = file.filePath;
            sourceDir = sourceDir.left(sourceDir.lastIndexOf("/") + 1);
            targetDir = this->targetPath+fileName;
            targetDir = targetDir.left(targetDir.lastIndexOf("/") + 1);
            QString tmp = file.filePath;
            tmp.remove(sourceDir);
            emit this->nextFile(tmp, sourceDir, targetDir, fileSize, counter);
            dialogKopiuj::fileRemove(this->targetPath+fileName, this->mode);
            if(!dir.mkpath(targetDir)){
                 qDebug()<<"Copy - Error while creating folder "<<this->targetPath.toStdString().c_str();
            }
            command = "\""+adb+"\""+" pull \""+codec->toUnicode(file.filePath.toUtf8())+"\" \""+codec->toUnicode(this->targetPath.toUtf8())+codec->toUnicode(fileName.toUtf8())+"\"";
            qDebug()<<"Copy - "<<command.toStdString().c_str();
            proces->start(command);
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Copy - "<<output.toStdString().c_str();
            delete proces;
        }
        emit this->copied();

        delete this->fileList;
    }
    else if (this->mode == dialogKopiuj::ComputerToPhone)
    {
        while (this->fileList->size() > 0)
        {
            proces = new QProcess;
            proces->setProcessChannelMode(QProcess::MergedChannels);
            counter++;
            file = this->fileList->takeFirst();
            if (file.filePath.endsWith(".apk") && !file.fileName.endsWith(".apk"))
            {
                fileName = file.fileName;
                fileName.replace(" ", "_");
                fileName.replace(QRegExp("\\W"), ".");
                fileName.append(".apk");
            }
            else
            {
                fileName = file.filePath;
                fileName.remove(this->sourcePath);
            }
            fileSize = file.fileSize.toInt();
            sourceDir = file.filePath;
            sourceDir = sourceDir.left(sourceDir.lastIndexOf("/") + 1);
            targetDir = this->targetPath+fileName;
            targetDir = targetDir.left(targetDir.lastIndexOf("/") + 1);
            emit this->nextFile(fileName, sourceDir, targetDir, fileSize, counter);
            dialogKopiuj::fileRemove(codec->toUnicode(this->targetPath.toUtf8())+
                                     codec->toUnicode(fileName.toUtf8()), this->mode);
            command = "\""+adb+"\""+" push \""+codec->toUnicode(file.filePath.toUtf8())+"\" \""+codec->toUnicode(this->targetPath.toUtf8())+ codec->toUnicode(fileName.toUtf8())+"\"";
            qDebug()<<"Copy - "<<command;
            proces->kill();
            qDebug()<<"Copy - process killed";
            proces->start(command);
            qDebug()<<"Copy - process started";
            proces->waitForFinished(-1);
            qDebug()<<"Copy - reading process";
            output = proces->readAll();
            qDebug()<<"Copy - "<<output;
            delete proces;
        }
        emit this->copied();

        delete this->fileList;
    }
    else if (this->mode == dialogKopiuj::PhoneToPhone)
    {
        while (this->fileList->size() > 0)
        {
            proces = new QProcess;
            proces->setProcessChannelMode(QProcess::MergedChannels);
            counter++;
            file = this->fileList->takeFirst();
            fileName = file.filePath;
            fileName.remove(this->sourcePath);
            fileSize = file.fileSize.toInt();
            sourceDir = file.filePath;
            sourceDir = sourceDir.left(sourceDir.lastIndexOf("/") + 1);
            targetDir = this->targetPath+fileName;
            targetDir = targetDir.left(targetDir.lastIndexOf("/") + 1);
            emit this->nextFile(file.fileName, sourceDir, targetDir, fileSize, counter);
            if (fileName.contains("/"))
            {
                proces->start("\""+adb + "\" shell " +  busybox + " mkdir \""+ codec->toUnicode(this->targetPath.toUtf8())
                             +codec->toUnicode(fileName.left(fileName.lastIndexOf("/")).toUtf8())+"\"");
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Copy - "<<output;
            }
            dialogKopiuj::fileRemove(codec->toUnicode(this->targetPath.toUtf8())+codec->toUnicode(fileName.toUtf8()), this->mode);
            command = "\""+adb + "\" shell cp \""+codec->toUnicode(file.filePath.toUtf8())+"\" "+"\""+
                               codec->toUnicode(this->targetPath.toUtf8())+codec->toUnicode(fileName.toUtf8())+"\"";
            qDebug()<<"Copy - "<<command;
            proces->start(command);
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Copy - "<<output;
            delete proces;
        }
        emit this->copied();

        delete this->fileList;
    }
    else if (this->mode == dialogKopiuj::AppsToComputer)
    {
        while (this->appList->size() > 0)
        {
            proces = new QProcess;
            proces->setProcessChannelMode(QProcess::MergedChannels);
            counter++;
            app = this->appList->takeFirst();
            QString tmp=app.appVersion;
            if (tmp.length()>15)
                tmp=tmp.left(15)+"...";
            fileName = app.appName;
            fileName.append("_" + tmp + ".apk");
            fileName.replace(" ", "_");
            fileName.replace(QRegExp("\\W"), ".");

            sourceDir = app.appFile;
            sourceDir = sourceDir.left(sourceDir.lastIndexOf("/") + 1);
            targetDir = this->targetPath+fileName;
            targetDir = targetDir.left(targetDir.lastIndexOf("/") + 1);
            emit this->nextFile(app.appFileName, sourceDir, targetDir, app.appSize.toInt(), counter);
            dialogKopiuj::fileRemove(this->targetPath+fileName, this->mode);
            command = "\""+adb+"\""+"adb pull \""+codec->toUnicode(app.appFile.toUtf8())+"\" \""+
                      codec->toUnicode(this->targetPath.toUtf8())+codec->toUnicode(fileName.toUtf8())+"\"";
            qDebug()<<"Copy - "<<command;
            proces->start(command);
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Copy - "<<output;
            delete proces;
        }
        emit this->copied();
        delete this->appList;
    }
}

void ThreadProgress::run()
{
    qDebug()<<"Copy, ThreadProgress.run() - START";
    QString command, output;
    int fileSize = 0;
    QStringList outputParts;
    QProcess *proces = new QProcess;
    QFile plik;
    proces->setProcessChannelMode(QProcess::MergedChannels);
//    QTextCodec *codec = QTextCodec::codecForLocale();

    if (this->mode == dialogKopiuj::PhoneToComputer || this->mode == dialogKopiuj::AppsToComputer)
    {
        while (this->maxSize > fileSize)
        {
            plik.setFileName(this->filePath);
            fileSize = plik.size();
            emit this->progressValue(fileSize);
            this->msleep(100);
        }
    }
    else if ((this->mode == dialogKopiuj::ComputerToPhone) || (this->mode == dialogKopiuj::PhoneToPhone))
    {
//        while (this->maxSize > fileSize)
//        {
//            command = "\""+sdk+"\""+"adb shell ls -l \""+codec->toUnicode(this->filePath.toUtf8())+"\"";
//            qDebug()<<"Copy, ThreadProgress.run() - startProces: "<<command;
//            proces->start(command);
//            proces->waitForFinished(1000);
//            proces->terminate();
//            output = proces->readAll();
//            outputParts = output.split(QRegExp("\\s+"), QString::SkipEmptyParts);
//            if (outputParts.size() >= 5)
//            {
//                fileSize = outputParts.at(4).toInt();
//                emit this->progressValue(fileSize);
//            }
//            msleep(500);
//        }
    }
    qDebug()<<"Copy progress.run() - END";
    delete proces;
}

QString dialogKopiuj::humanReadableSize(QString size)
{
    long double sizeTmp = 0.0;
    QString strTmp = " B";
    sizeTmp = size.toLongLong();
    if (sizeTmp <= 1024)
    {
        strTmp.prepend(QString::number(sizeTmp, 'f', 0));
        return strTmp;
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " KiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " MiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " GiB";
    }
    strTmp.prepend(QString::number(sizeTmp, 'f', 2));
    return strTmp;
}

void dialogKopiuj::fileRemove(QString filePath, int mode)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    if (mode == PhoneToComputer || mode == AppsToComputer)
    {
        QFileInfo fInfo;
        fInfo.setFile(filePath);
        if (fInfo.exists())
            QFile::remove(filePath);
    }
    else if (mode == PhoneToPhone || mode == ComputerToPhone)
    {
        QProcess *proces = new QProcess;
        QString command = "\""+adb+"\""+" shell rm -f \""+codec->toUnicode(filePath.toUtf8())+"\"";
        proces->start(command);
        proces->waitForFinished(-1);
    }
}
