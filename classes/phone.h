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


#ifndef PHONE_H
#define PHONE_H

#include <QProcess>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QTcpSocket>
#include <QThread>
#include <QIcon>
#include <QDebug>
#include <QDateTime>
#include <QTextCodec>
#include "./models/filetablemodel.h"

#define DISCONNECTED 0
#define DEVICE 1
#define RECOVERY 2
#define FASTBOOT 3

typedef struct fileList
{
    QStringList date;
    QStringList name;
    QStringList size;
    QStringList type;
    QStringList extension;
    QList<QIcon> icon;
} FileList;

class ConnectionThread : public QThread
{
    Q_OBJECT
public:
    void run();

    QTcpSocket *socket;
signals:
    void connectionChanged(int,QString);
};

class Phone: public QObject
{
    Q_OBJECT
public:
    Phone(QString sdk,bool isThreadNecessary);
    ~Phone();

    bool cd(QString dir);
    bool cdUp();
//    FileList fileList;
    int getConnectionState();
    QString getConnectionMode();
//    FileList getFileList();
    QList<File> *getFileList();
    QList<File> *getFileList(QString);
    static FileList *getStaticFileList(QString path, QString sdk, bool hiddenFiles);
    bool getHiddenFilesState();
    QString getPath();
    QImage getScreenshot();
    QString getSdk();
    bool makeDir(QString newDir);
    bool recoverySDmounted;
    bool remove(QString name);
    bool rename(QString oldName,QString newName);
    QString serialNumber;
    void setConnectionState(int);
    void setHiddenFiles(bool);
    static QString getIp();
    void setPath(QString newPath);
    ConnectionThread connectionThread;
    QTextCodec *codec;
    static QStringList getGoogleAccounts();
    bool procesEvents;

private:

    QString connecionMode;
    int connectionState;
    bool hiddenFiles;
    QString path;
    QString sdk;

public slots:
    void slotConnectionChanged(int connectionState,QString serialNumber);


    void adbRebootBootloader();
    void adbRebootRecovery();
    void adbReboot();
    void adbPowerOff();
    void fastbootReboot();
    void fastbootRebootBootloader();
    void fastbootPowerOff();


signals:
    void signalConnectionChanged(int connectionState);
};

#endif // PHONE_H
