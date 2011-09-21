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


#include "phone.h"
#include <QSettings>
#include <QTextCodec>


void ConnectionThread::run()
{
    int dataLength;
    bool ok, first = true;
    QString data,serialNumber,tmp;
    int serialLength=0;

    this->socket = new QTcpSocket();
    this->socket->connectToHost("127.0.0.1",5037,QTcpSocket::ReadWrite);
    if (this->socket->waitForConnected(2000))
    {
        this->socket->write("0012host:track-devices");
        this->socket->waitForReadyRead(2000);
        data = this->socket->read(4);
        if (data == "OKAY")
        {

            while (true)
            {
                if (!first)
                    this->socket->waitForReadyRead(-1);
                first = false;

                data = this->socket->read(4);
                tmp=data;
                if (data == "")
                    continue;
                dataLength = data.toInt(&ok, 16);
                if (dataLength == 0)
                {
                    emit this->connectionChanged(FASTBOOT,"");
                }
                else
                {
                    data = this->socket->read(dataLength);

                    if (data.contains("device"))
                        serialLength = tmp.toInt(&ok, 16) - 8;
                    if (data.contains("recovery"))
                        serialLength = tmp.toInt(&ok, 16) - 10;

                    serialNumber = data;

                    serialNumber = serialNumber.left(serialLength);
                    if (data.contains("device"))
                        emit this->connectionChanged(DEVICE,serialNumber);
                    if (data.contains("recovery"))
                        emit this->connectionChanged(RECOVERY,serialNumber);
                    if (data.contains("offline"))
                        emit this->connectionChanged(FASTBOOT,serialNumber);
                }
            }
        }
        else
        {
            emit this->connectionChanged(-1,"");
            return;
        }
    }
    else
    {
        emit this->connectionChanged(-1,"");
        return;
    }
}

Phone::Phone(QString sdk,bool isThreadNecessary)
{
    QProcess fastboot;
    this->sdk=sdk;
    this->codec = QTextCodec::codecForLocale();

    qDebug()<<"Phone::Phone - sdk="<<this->sdk;
    fastboot.setProcessChannelMode(QProcess::MergedChannels);
    fastboot.start("\"" + this->sdk + "\"adb remount");
    fastboot.waitForFinished();
    qDebug()<<"Phone::Phone - adb start-server: "<<fastboot.readAll();
    if (isThreadNecessary)
        this->connectionThread.start();
    connect(&this->connectionThread, SIGNAL(connectionChanged(int,QString)), this, SLOT(slotConnectionChanged(int,QString)));
    this->recoverySDmounted = false;
}

Phone::~Phone()
{
    if (this->connectionThread.isRunning())
        this->connectionThread.terminate();
}

bool Phone::cd(QString dir)
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;
    if (dir==".")
        dir="";
    QString oldPath=this->getPath();

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;
    if (dir.contains("/"))
    {
        command="\""+this->sdk+"\""+"adb shell cd \""+this->codec->toUnicode(dir.toUtf8())+"\"";
    }
    else
    {
        dir.prepend(this->getPath());
        command="\""+this->sdk+"\""+"adb shell cd \""+this->codec->toUnicode(dir.toUtf8())+"\"";
    }
    phone->start(command);
    phone->waitForReadyRead(-1);
    command=phone->readAll();

    if (!command.isEmpty())
    {
        this->setPath(oldPath);
        return false;
    }
    if (dir=="")
        return true;
    if (dir.contains("/"))
    {
        if (dir[dir.length()-1]!='/')
            dir.append('/');
        this->setPath(dir);
    }
    else
        this->setPath(oldPath+dir+"/");

    delete phone;
    return true;
}

bool Phone::cdUp()
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;

    if (this->path.count('/')>1)
    {
        this->path.chop(1);
        this->path.chop(this->path.length()-this->path.lastIndexOf('/'));
        this->path.append("/");
//        this->fileList = this->getFileList();
        return true;
    }
    else
        return false;
}

int Phone::getConnectionState()
{
    return this->connectionState;
}

QList<File> *Phone::getFileList()
{
    QList<File> *fileList = new QList<File>;
    File tmpFile;

    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
    {
        return NULL;
    }

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<this->getPath();
    if (this->hiddenFiles)
        command="\""+this->sdk+"\""+"adb shell \"busybox ls -l -a \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\'\"";
    else
        command="\""+this->sdk+"\""+"adb shell \"busybox ls -l \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\'\"";

    qDebug()<<"Phone::getFileList() - "<<command;
    phone->start(command);
    QString outputLine="1";
    QStringList outputLines;

//    while (!outputLine.isEmpty())
//    {
//        qApp->processEvents();
//        phone->waitForReadyRead(-1);
//        outputLine = phone->readLine();
//        qDebug()<<"Phone::getFileList() - "<<outputLine;
//        outputLines.append(outputLine);
//    }
    phone->waitForFinished(-1);
    outputLine=phone->readAll();
    outputLines=outputLine.split("\n");
    if (outputLines.first().contains("No such file or directory")||outputLines.first().contains("cannot")||outputLines.first().contains("Not a directory"))
    {
        qDebug()<<"Phone::getFileList() - "<<outputLine;
        phone->terminate();
        delete phone;
        return NULL;
    }

    outputLines.removeLast();   // pusty

    QStringList lineParts;
    QString name;
    QFile plik;
    QFileIconProvider *provider = new QFileIconProvider;
    QString tmp;

    while (outputLines.length()>0)
    {
        qApp->processEvents();
        lineParts.clear();
        name.clear();
        tmp.clear();
        tmp = outputLines.takeFirst();
        tmp.remove(QRegExp("\\s+$"));
        lineParts=tmp.split(QRegExp("\\s+"));
        if (lineParts.length()>8)
        {
            if (lineParts[4].contains(","))
            {
                tmpFile.fileSize = lineParts.at(4)+lineParts.at(5);
                lineParts.removeAt(5);
            }
            else
                tmpFile.fileSize = lineParts.at(4);
            tmpFile.fileDate = lineParts[5]+" "+lineParts[6]+" "+lineParts[7];

            for (int i=8;i<lineParts.length();i++)
                name.append(lineParts.at(i)+" ");
            name.chop(1);
            name.remove(QString("%1[0m").arg( QChar( 0x1b )));
            name.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            if (name.contains("0;30"))//black
            {
                tmpFile.fileColor = QColor(Qt::black);
            }
            else if (name.contains("0;34"))//blue
            {
                tmpFile.fileColor = QColor(Qt::blue);
            }
            else if (name.contains("0;32"))//green
            {
                tmpFile.fileColor = QColor(Qt::green);
            }
            else if (name.contains("0;36"))//cyan
            {
                tmpFile.fileColor = QColor(Qt::cyan);
            }
            else if (name.contains("0;31"))//red
            {
                tmpFile.fileColor = QColor(Qt::red);
            }
            else if (name.contains("0;35"))//purple
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("0;33"))//brown
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("0;37"))//light gray
            {
                tmpFile.fileColor = QColor(Qt::lightGray);
            }
            else if (name.contains("1;30"))//dark gray
            {
                tmpFile.fileColor = QColor(Qt::darkGray);
            }
            else if (name.contains("1;34"))//dark gray
            {
                tmpFile.fileColor = QColor(Qt::blue);
            }
            else if (name.contains("1;32"))//light green
            {
                tmpFile.fileColor = QColor(Qt::green);
            }
            else if (name.contains("1;36"))//light cyan
            {
                tmpFile.fileColor = QColor(Qt::cyan);
            }
            else if (name.contains("1;31"))//light red
            {
                tmpFile.fileColor = QColor(Qt::red);
            }
            else if (name.contains("1;35"))//light purple
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("1;33"))//yellow
            {
                tmpFile.fileColor = QColor(Qt::yellow);
            }
            else if (name.contains("1;37"))//white
            {
                tmpFile.fileColor = QColor(Qt::white);
            }
            else
                tmpFile.fileColor = QColor(Qt::black);
            name.remove(QRegExp("\\[\\d;\\d+m"));

            tmpFile.fileName = QString::fromUtf8(name.toAscii());
            tmpFile.filePath = this->getPath() + tmpFile.fileName;

            qDebug()<<"Phone::getFileList() - plik: "<<name<< " - " <<lineParts.first();

            if (lineParts.first()[0]=='d')
                tmpFile.fileType = "dir";
            else if (lineParts.first()[0]=='-'||lineParts.first()[0]=='s')
                tmpFile.fileType = "file";
            else if (lineParts.first()[0]=='l')
                tmpFile.fileType = "link";
            else if (lineParts.first()[0]=='c'||lineParts.first()[0]=='b'||lineParts.first()[0]=='p')
                tmpFile.fileType = "device";

            name = tmpFile.fileName;
            name.remove(QString("%1[0m").arg( QChar( 0x1b )));
            name.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            name.remove(QRegExp("\\[\\d;\\d+m"));
            if (tmpFile.fileType == "file" || tmpFile.fileType == "device")
            {
                plik.setFileName(QDir::currentPath()+"/tmp/"+name);
                plik.open(QFile::WriteOnly);
                tmpFile.fileIcon = provider->icon(QFileInfo(plik));
                plik.remove();
            }
            else if (tmpFile.fileType == "link")
            {
                tmpFile.fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
            }
            else
                tmpFile.fileIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);

            if (tmpFile.fileName == "." || tmpFile.fileName == "..")
                continue;
            else
                fileList->append(tmpFile);
        }
    }
    phone->close();
    qDebug()<<"Phone::getFileList() - skonczylem analizowac pliki";

    phone->terminate();
    delete provider;
    delete phone;
    return fileList;
}

QList<File> *Phone::getFileList(QString filter)
{
    QList<File> *fileList = new QList<File>;
    File tmpFile;

    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
    {
        return NULL;
    }

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<this->getPath();
    if (this->hiddenFiles)
        command="\""+this->sdk+"\""+"adb shell \"busybox ls -l -a \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\' | grep \'" + filter + "\'\"";
    else
        command="\""+this->sdk+"\""+"adb shell \"busybox ls -l \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\' | grep \'" + filter + "\'\"";

    qDebug()<<"Phone::getFileList() - "<<command;
    phone->start(command);
    QString outputLine="1";
    QStringList outputLines;

    while (!outputLine.isEmpty())
    {
        qApp->processEvents();
        phone->waitForReadyRead(-1);
        outputLine = phone->readLine();
        qDebug()<<"Phone::getFileList() - "<<outputLine;
        outputLines.append(outputLine);
    }
    if (outputLines.first().contains("No such file or directory")||outputLines.first().contains("cannot")||outputLines.first().contains("Not a directory"))
    {
        phone->terminate();
        delete phone;
        return NULL;
    }

    outputLines.removeLast();   // pusty

    QStringList lineParts;
    QString name;
    QFile plik;
//    QFileIconProvider *provider = new QFileIconProvider;
    QString tmp;

    while (outputLines.length()>0)
    {
        qApp->processEvents();
        lineParts.clear();
        name.clear();
        tmp.clear();
        tmp = outputLines.takeFirst();
        tmp.remove(QRegExp("\\s+$"));
        lineParts=tmp.split(QRegExp("\\s+"));
//        outputLines.removeFirst();
        if (lineParts.length()>8)
        {
            if (lineParts[4].contains(","))
            {
                tmpFile.fileSize = lineParts.at(4)+lineParts.at(5);
                lineParts.removeAt(5);
            }
            else
                tmpFile.fileSize = lineParts.at(4);
            tmpFile.fileDate = lineParts[5]+" "+lineParts[6]+" "+lineParts[7];

            for (int i=8;i<lineParts.length();i++)
                name.append(lineParts.at(i)+" ");
            name.chop(1);
            name.remove(QString("%1[0m").arg( QChar( 0x1b )));
            name.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            if (name.contains("0;30"))//black
            {
                tmpFile.fileColor = QColor(Qt::black);
            }
            else if (name.contains("0;34"))//blue
            {
                tmpFile.fileColor = QColor(Qt::blue);
            }
            else if (name.contains("0;32"))//green
            {
                tmpFile.fileColor = QColor(Qt::green);
            }
            else if (name.contains("0;36"))//cyan
            {
                tmpFile.fileColor = QColor(Qt::cyan);
            }
            else if (name.contains("0;31"))//red
            {
                tmpFile.fileColor = QColor(Qt::red);
            }
            else if (name.contains("0;35"))//purple
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("0;33"))//brown
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("0;37"))//light gray
            {
                tmpFile.fileColor = QColor(Qt::lightGray);
            }
            else if (name.contains("1;30"))//dark gray
            {
                tmpFile.fileColor = QColor(Qt::darkGray);
            }
            else if (name.contains("1;34"))//dark gray
            {
                tmpFile.fileColor = QColor(Qt::blue);
            }
            else if (name.contains("1;32"))//light green
            {
                tmpFile.fileColor = QColor(Qt::green);
            }
            else if (name.contains("1;36"))//light cyan
            {
                tmpFile.fileColor = QColor(Qt::cyan);
            }
            else if (name.contains("1;31"))//light red
            {
                tmpFile.fileColor = QColor(Qt::red);
            }
            else if (name.contains("1;35"))//light purple
            {
                tmpFile.fileColor = QColor(0, 0, 0);
            }
            else if (name.contains("1;33"))//yellow
            {
                tmpFile.fileColor = QColor(Qt::yellow);
            }
            else if (name.contains("1;37"))//white
            {
                tmpFile.fileColor = QColor(Qt::white);
            }
            else
                tmpFile.fileColor = QColor(Qt::black);
            name.remove(QRegExp("\\[\\d;\\d+m"));

            tmpFile.fileName = QString::fromUtf8(name.toAscii());
            tmpFile.filePath = this->getPath() + tmpFile.fileName;

            qDebug()<<"Phone::getFileList() - plik: "<<name<< " - " <<lineParts.first();

            if (lineParts.first()[0]=='d')
                tmpFile.fileType = "dir";
            else if (lineParts.first()[0]=='-'||lineParts.first()[0]=='s')
                tmpFile.fileType = "file";
            else if (lineParts.first()[0]=='l')
                tmpFile.fileType = "link";
            else if (lineParts.first()[0]=='c'||lineParts.first()[0]=='b'||lineParts.first()[0]=='p')
                tmpFile.fileType = "device";

            name = tmpFile.fileName;
            name.remove(QString("%1[0m").arg( QChar( 0x1b )));
            name.remove(QChar( 0x1b ), Qt::CaseInsensitive);
            name.remove(QRegExp("\\[\\d;\\d+m"));
//            if (tmpFile.fileType == "file" || tmpFile.fileType == "device")
//            {
//                plik.setFileName(QDir::currentPath()+"/tmp/"+name);
//                plik.open(QFile::WriteOnly);
//                tmpFile.fileIcon = provider->icon(QFileInfo(plik));
//                plik.remove();
//            }
//            else if (tmpFile.fileType == "link")
//            {
//                tmpFile.fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileLinkIcon);
//            }
//            else
//                tmpFile.fileIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);

            if (tmpFile.fileName == "." || tmpFile.fileName == "..")
                continue;
            else
                fileList->append(tmpFile);
        }
    }
    phone->close();
    qDebug()<<"Phone::getFileList() - skonczylem analizowac pliki";

    phone->terminate();
//    delete provider;
    delete phone;
    return fileList;
}

FileList *Phone::getStaticFileList(QString path, QString sdk, bool hiddenFiles)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    FileList *fileList = new FileList;

    QProcess *phone=new QProcess;
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<path;
    if (hiddenFiles)
        command="\""+sdk+"\""+"adb shell \"busybox ls -l -a \'"+codec->toUnicode(path.toAscii())+"\'\"";
    else
        command="\""+sdk+"\""+"adb shell \"busybox ls -l \'"+codec->toUnicode(path.toAscii())+"\'\"";

    qDebug()<<"Phone::getFileList() - "<<command;
    phone->start(command);
    QString outputLine="1";
    QStringList outputLines;

    while (!outputLine.isEmpty())
    {
        phone->waitForReadyRead(-1);
        outputLine = phone->readLine();
        qDebug()<<"Phone::getFileList() - "<<outputLine;
        outputLines.append(outputLine);
    }
    if (outputLines.first().contains("No such file or directory")||outputLines.first().contains("cannot")||outputLines.first().contains("Not a directory"))
    {
        fileList->name.append("error");
        phone->terminate();
        delete phone;
        return fileList;
    }

    outputLines.removeLast();   // pusty

    QStringList lineParts;
    QString name;

    while (outputLines.length()>0)
    {
        lineParts.clear();
        name.clear();
        lineParts=outputLines.first().split(QRegExp("\\s+"));
        outputLines.removeFirst();
        if (lineParts.length()>8)
        {
            if (lineParts[4].contains(","))
            {
                fileList->size.append(lineParts.at(4)+lineParts.at(5));
                lineParts.removeAt(5);
            }
            else
                fileList->size.append(lineParts.at(4));
            fileList->date.append(lineParts[5]+" "+lineParts[6]+" "+lineParts[7]);

            for (int i=8;i<lineParts.length()-1;i++)
                name.append(lineParts.at(i)+" ");
            name.chop(1);
            fileList->name.append(name);
            qDebug()<<"Phone::getFileList() - plik: "<<name<< " - " <<lineParts.first();


            if (lineParts.first()[0]=='d')
                fileList->type.append("dir");
            else if (lineParts.first()[0]=='-'||lineParts.first()[0]=='s')
                fileList->type.append("file");
            else if (lineParts.first()[0]=='l')
                fileList->type.append("link");
            else if (lineParts.first()[0]=='c'||lineParts.first()[0]=='b'||lineParts.first()[0]=='p')
                fileList->type.append("device");
        }
    }
    phone->close();
    qDebug()<<"Phone::getFileList() - skonczylem analizowac pliki";
    if (fileList->name.count()>0)
    {
        //        if (fileList->name.first()==".")
        //        {
        int x=fileList->name.indexOf(".");
        if (x!=-1)
        {
            qDebug("deleting .");
            fileList->date.removeAt(x);
            fileList->name.removeAt(x);
            fileList->size.removeAt(x);
            fileList->type.removeAt(x);
        }

        x=fileList->name.indexOf("..");
        if (x!=-1)
        {
            qDebug("deleting ..");
            fileList->date.removeAt(x);
            fileList->name.removeAt(x);
            fileList->size.removeAt(x);
            fileList->type.removeAt(x);
        }
    }
    qDebug()<<"Phone::getFileList() - . i .. usuniete, koncze funkcje";
    phone->terminate();
    delete phone;
    return fileList;
}

bool Phone::getHiddenFilesState()
{
    return this->hiddenFiles;
}

QString Phone::getPath()
{
    if (this->path.endsWith("/"))
        return this->path;
    else
        return this->path + "/";
}

QString Phone::getSdk()
{
    return this->sdk;
}

bool Phone::makeDir(QString newDir)
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    newDir.prepend(this->getPath());
    command="\""+this->sdk+"\""+"adb shell busybox mkdir \""+this->codec->toUnicode(newDir.toUtf8())+"\"";
    phone->start(command);

    phone->waitForReadyRead(-1);
    QString outputLine=phone->readLine();

    phone->terminate();
    delete phone;

    if (outputLine.contains(QRegExp("can.+t create directory")))
        return false;
    return true;
}

void Phone::setConnectionState(int state)
{
    this->connectionState=state;
}

void Phone::setHiddenFiles(bool hiddenFiles)
{
    this->hiddenFiles=hiddenFiles;
}

void Phone::setPath(QString newPath)
{
    this->path=newPath;
}

void Phone::slotConnectionChanged(int connState,QString serialNumber)
{
    QString output="";
    if (connState == DISCONNECTED)
    {
        this->connectionState = DISCONNECTED;
        this->serialNumber.clear();
        qDebug()<<"Phone::slotConnectionChanged - DISCONNECTED";
        emit this->signalConnectionChanged(this->connectionState);
        return;
    }
    if (connState == RECOVERY)
    {
        this->connecionMode = "usb";
        this->serialNumber=serialNumber;
        this->connectionState = RECOVERY;
        qDebug()<<"Phone::slotConnectionChanged - RECOVERY";
        emit this->signalConnectionChanged(this->connectionState);
        return;
    }
    if (connState == DEVICE)
    {
        this->serialNumber=serialNumber;
        if (this->serialNumber.contains(".") && this->serialNumber.contains(":"))
            this->connecionMode = "wifi";
        else
            this->connecionMode = "usb";
        this->connectionState = DEVICE;
        qDebug()<<"Phone::slotConnectionChanged - DEVICE("<<this->connecionMode<<")";
        emit this->signalConnectionChanged(this->connectionState);
        return;
    }
    if (connState == FASTBOOT)
    {
        this->connecionMode = "usb";
        this->serialNumber=serialNumber;
        QProcess fastboot;
        fastboot.setProcessChannelMode(QProcess::MergedChannels);
        fastboot.start("\"" + this->sdk + "\"fastboot devices");
        fastboot.waitForFinished();
        output = fastboot.readAll();
        if (output.contains("fastboot"))
        {
            this->connectionState = FASTBOOT;
            qDebug()<<"Phone::slotConnectionChanged - FASTBOOT";
            emit this->signalConnectionChanged(this->connectionState);
            return;
        }
        else
        {
            this->serialNumber.clear();
            qDebug()<<"Phone::slotConnectionChanged - DISCONNECTED";
            this->connectionState = DISCONNECTED;
            emit this->signalConnectionChanged(this->connectionState);
            return;
        }
    }
    if (connState == -1)
    {
        this->connectionThread.terminate();
        this->connectionThread.start();
    }
    return;
}

QString Phone::getConnectionMode()
{
    return this->connecionMode;
}

bool Phone::remove(QString name)
{
//    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
//        return false;

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    command="\""+this->sdk+"\""+"adb shell busybox rm -r "+"\""+this->codec->toUnicode(this->getPath().toUtf8())+
            this->codec->toUnicode(name.toUtf8())+"\"";
    phone->start(command);

    phone->waitForReadyRead(-1);
    QString outputLine=phone->readLine();

    phone->terminate();
    delete phone;

    if (outputLine.contains("cannot remove"))
        return false;
    return true;
}

bool Phone::rename(QString oldName, QString newName)
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;

    QProcess *phone=new QProcess(this);
    phone->setProcessChannelMode(QProcess::MergedChannels);
    QString command;

    command="\""+this->sdk+"\""+"adb shell busybox mv \""+this->codec->toUnicode(this->getPath().toUtf8())+this->codec->toUnicode(oldName.toUtf8())+
            "\" \""+this->codec->toUnicode(this->getPath().toUtf8())+this->codec->toUnicode(newName.toUtf8())+"\"";
    phone->start(command);

    phone->waitForReadyRead(-1);
    QString outputLine=phone->readLine();

    phone->terminate();
    delete phone;

    if (outputLine.contains("cannot rename"))
        return false;
    return true;
}

void Phone::adbPowerOff()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"adb shell shutdown");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
}

void Phone::adbReboot()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"adb shell reboot");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
}

void Phone::adbRebootBootloader()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"adb shell reboot bootloader");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
}

void Phone::adbRebootRecovery()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"adb shell reboot recovery");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
}

void Phone::fastbootPowerOff()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"fastboot oem powerdown");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

void Phone::fastbootReboot()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"fastboot reboot");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

void Phone::fastbootRebootBootloader()
{
    QProcess *reboot=new QProcess();
    reboot->start("\""+sdk+"\"fastboot reboot-bootloader");
    reboot->waitForFinished(-1);
    reboot->terminate();
    delete reboot;
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

QStringList Phone::getGoogleAccounts()
{

    QString sdk;
    QString output;
    QSettings settings;
    sdk = settings.value("sdkPath").toString();
    QString operation = "\""+sdk+"\""+ "adb shell busybox grep gmail-ls /data/system/sync/accounts.xml";
//    QString operation = "\""+sdk+"\""+ "adb shell su -c 'busybox grep gmail-ls /data/system/sync/accounts.xml'";
//su -c 'busybox grep gmail-ls /data/system/sync/accounts.xml'
    QProcess *proces=new QProcess;
    proces->start(operation);
    proces->waitForFinished(-1);
    output = proces->readAll();
    qDebug()<<"Phone::getGoogleAccounts(): "<<output;
    delete proces;
    if (output.contains("permission") && !output.contains("account"))//////////////////////////////zmeinic
    {
        operation = "\""+sdk+"\""+ "adb shell su -c 'busybox grep gmail-ls /data/system/sync/accounts.xml'";
        proces = new QProcess;
        proces->start(operation);
        proces->waitForFinished(-1);
        output = proces->readAll();
        delete proces;
    }

    QStringList list = output.split("\n");
    QStringList result;
    foreach(QString element, list)
    {
        element.remove(QRegExp(".*account=\""));
        element.remove(QRegExp("\".*"));
        result.append(element);
    }

    return result;
}

QString Phone::getIp()
{//? (192.168.2.1) at d8:5d:4c:de:85:94 [ether]  on tiwlan0
    QProcess *proces=new QProcess;
    QSettings settings;
    proces->start("\"" + settings.value("sdkPath").toString() + "\"adb shell busybox arp");
    QString tmp;
    proces->waitForFinished(-1);
    tmp = proces->readAll();
    qDebug()<<"Phone::getIP(): "<<tmp;

    if (tmp.length() < 5)
    {
        qDebug()<<"Phone::getIP(): wlan is off";
        return "";
    }
    tmp.remove(0,tmp.lastIndexOf(" "));
    tmp.remove("\n");

    proces->start("\"" + settings.value("sdkPath").toString() + "\"adb shell busybox ifconfig "+tmp);
    proces->waitForFinished(-1);
    tmp = proces->readAll();
    tmp.remove(QRegExp(".*inet addr:"));
    tmp.remove(QRegExp("\\s.*"));

    qDebug()<<"Phone::getIP(), gotIP:  "<<tmp;
    delete proces;
    return tmp;
}
