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
#include "sockets.h"

extern QString sdk;
extern QString adb;
extern QString aapt;
extern QProcess *adbProces;
extern QString busybox;
extern QString fastboot;
extern Socket socket_global;

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

Phone::Phone(bool isThreadNecessary)
{
    QProcess proces;
    this->codec = QTextCodec::codecForLocale();

    qDebug()<<"Phone::Phone - sdk="<<sdk.toStdString().c_str();
    proces.setProcessChannelMode(QProcess::MergedChannels);
    proces.start("\"" + adb + "\" remount");

    proces.waitForFinished();
    qDebug()<<"Phone::Phone - adb start-server: "<<proces.readAll();
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
        command="\""+ adb + "\" shell cd \""+this->codec->toUnicode(dir.toUtf8())+"\"";
    }
    else
    {
        dir.prepend(this->getPath());
        command="\""+ adb +"\" shell cd \""+this->codec->toUnicode(dir.toUtf8())+"\"";
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
    QString outputLine="1";
    QStringList outputLines;

    QString command;
    command =  command="\""+adb+"\" shell busybox ";
    command = QDir::toNativeSeparators(command);
    qDebug()<<"Phone::getFileList() - "<<command.toStdString().c_str();

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<this->getPath();
    outputLine= socket_global.listFiles(this->codec->toUnicode(this->getPath().toUtf8()));
    if (outputLine.isEmpty())
    {
        return NULL;
    }
    outputLines=outputLine.split("\n");
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
        lineParts=tmp.split("\t");
        if((lineParts.count()>3)&&(((QString)lineParts[0]).length()>14))
        {
            tmpFile.fileSize = lineParts.at(1);
            name.append(lineParts.at(3));
            tmpFile.filePermissions = lineParts[0].right(9);
            tmpFile.fileColor = QColor(Qt::black);
            if(tmpFile.filePermissions.length()<2) tmpFile.filePermissions = "111111111";

            tmpFile.fileName = QString::fromUtf8(name.toAscii());
            tmpFile.filePath = this->getPath() + tmpFile.fileName;
            tmpFile.fileDate = lineParts[2];
            qDebug()<<"Phone::getFileList() - plik: "<<name<< " - " <<lineParts.first();

            if (((QString)lineParts[0]).length() == 15){
                tmpFile.fileName = tmpFile.fileName;
                tmpFile.fileType = File::dir;
            } else if (((QString)(lineParts[0]))[2]=='1')
                tmpFile.fileType = File::link;
//            else if (((QString)(lineParts[0]))[2]=='0') // TODO: FIX ME!!!
//                tmpFile.fileType = File::device;
            else if (((QString)(lineParts[0]))[2]=='0') {
                tmpFile.fileType = File::file;
                if (tmpFile.filePermissions[0] == '0') //Not readable
                {
                    tmpFile.fileColor = QColor(Qt::darkRed);
                }
                else if (tmpFile.filePermissions[1] == '0')//Not writable
                {
                    tmpFile.fileColor = QColor(Qt::darkBlue);
                }
                else if (tmpFile.filePermissions[2] == '1')//Executable
                {
                    tmpFile.fileColor = QColor(Qt::darkGreen);
                }
            }
            name = tmpFile.fileName;

            if (tmpFile.fileType == File::file || tmpFile.fileType == File::device)
            {
                plik.setFileName(QDir::currentPath()+"/tmp/"+name);
                plik.open(QFile::WriteOnly);
                tmpFile.fileIcon = provider->icon(QFileInfo(plik));
                plik.remove();
            }
            else if (tmpFile.fileType == File::link)
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
    qDebug()<<"Phone::getFileList() - skonczylem analizowac pliki";

    delete provider;
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
    QString outputLine="1";
    QStringList outputLines;

    QString command;

    command =  command="\""+adb+"\" shell \"" + busybox + " ls";
    qDebug()<<"Phone::getFileList() - "<<command;
    adbProces->start(command);
    adbProces->waitForFinished(-1);
    outputLine=adbProces->readAll();
    outputLines=outputLine.split("\n");
    if (outputLines.first().contains("not")){
        busybox = "";
    } else {
        busybox = "busybox";
    }

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<this->getPath().toStdString().c_str();
    if (this->hiddenFiles)
        command="\""+adb+"\" shell \"" + busybox + " ls -l -a \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\' | grep \'" + filter + "\'\"";
    else
        command="\""+adb+"\" shell \"" + busybox + " ls -l \'"+this->codec->toUnicode(this->getPath().toUtf8())+"\' | grep \'" + filter + "\'\"";

    qDebug()<<"Phone::getFileList() - "<<command.toStdString().c_str();
    adbProces->start(command);


    while (!outputLine.isEmpty())
    {
        qApp->processEvents();
        adbProces->waitForReadyRead(-1);
        outputLine = adbProces->readLine();
        qDebug()<<"Phone::getFileList() - "<<outputLine;
        outputLines.append(outputLine);
    }
    if (outputLines.first().contains("No such file or directory")||outputLines.first().contains("cannot")||outputLines.first().contains("Not a directory"))
    {
        adbProces->terminate();
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
                tmpFile.fileType = File::dir;
            else if (lineParts.first()[2]=='0')
                tmpFile.fileType = File::file;
            else if (lineParts.first()[2]=='l')
                tmpFile.fileType = File::link;
            else if (lineParts.first()[0]=='c'||lineParts.first()[0]=='b'||lineParts.first()[0]=='p')
                tmpFile.fileType = File::device;

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
    adbProces->close();
    qDebug()<<"Phone::getFileList() - skonczylem analizowac pliki";

    adbProces->terminate();
//    delete provider;
    return fileList;
}

FileList *Phone::getStaticFileList(QString path, QString adb, bool hiddenFiles)
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    FileList *fileList = new FileList;
    QString outputLine="1";
    QStringList outputLines;

    QString command;

    QString busyBoxStr;
    command =  command="\"" + adb +"\" shell busybox ls";
    qDebug()<<"Phone::getFileList() - "<<command.toStdString().c_str();
    adbProces->start(command);
    adbProces->waitForFinished(-1);
    outputLine=adbProces->readAll();
    outputLines=outputLine.split("\n");
    if (outputLines.first().contains("not")){
        busyBoxStr = "";
    } else {
        busyBoxStr = "busybox";
    }

    qDebug()<<QDateTime::currentDateTime().toString("hh:mm:ss");
    qDebug()<<"Phone::getFileList() - "<<path;
    if (hiddenFiles)
        command="\""+adb+"\"", QStringList() <<"shell \"" + busyBoxStr + " ls -l -a \'"+codec->toUnicode(path.toAscii())+"\'\"";
    else
        command="\""+adb+"\"", QStringList() <<"shell \"" + busyBoxStr + " ls -l \'"+codec->toUnicode(path.toAscii())+"\'\"";

    qDebug()<<"Phone::getFileList() - "<<command.toStdString().c_str();
    adbProces->start(command);


    while (!outputLine.isEmpty())
    {
        adbProces->waitForReadyRead(-1);
        outputLine = adbProces->readLine();
        qDebug()<<"Phone::getFileList() - "<<outputLine;
        outputLines.append(outputLine);
    }
    if (outputLines.first().contains("No such file or directory")||outputLines.first().contains("cannot")||outputLines.first().contains("Not a directory"))
    {
        fileList->name.append("error");
        adbProces->terminate();
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
    adbProces->close();
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
    adbProces->terminate();
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

bool Phone::makeDir(QString newDir)
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;

    QString command;

    newDir.prepend(this->getPath());
    command="\""+adb+"\" shell " + busybox + " mkdir \""+this->codec->toUnicode(newDir.toUtf8())+"\"";
    adbProces->start(command);

    adbProces->waitForReadyRead(-1);
    QString outputLine=adbProces->readLine();

    adbProces->terminate();

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
        QProcess fastboot_proc;
        fastboot_proc.setProcessChannelMode(QProcess::MergedChannels);
        fastboot_proc.start("\"" + fastboot + "\" devices");
        fastboot_proc.waitForFinished();
        output = fastboot_proc.readAll();
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

    adbProces->start("\""+ adb +"\" shell " + busybox + QString(" rm -r ")+"\""
                     +this->codec->toUnicode(this->getPath().toUtf8())+
                     this->codec->toUnicode(name.toUtf8())+"\"");

    adbProces->waitForReadyRead(-1);
    QString outputLine=adbProces->readLine();

    adbProces->terminate();

    if (outputLine.contains("cannot remove"))
        return false;
    return true;
}

bool Phone::rename(QString oldName, QString newName)
{
    if ((this->getConnectionState() != RECOVERY) && (this->getConnectionState() != DEVICE))
        return false;

    QString command;

    command="\"" + adb + "\" shell " + busybox + " mv \""+this->codec->toUnicode(this->getPath().toUtf8())+this->codec->toUnicode(oldName.toUtf8())+
            "\" \""+this->codec->toUnicode(this->getPath().toUtf8())+this->codec->toUnicode(newName.toUtf8())+"\"";
    adbProces->start(command);

    adbProces->waitForReadyRead(-1);
    QString outputLine=adbProces->readLine();

    adbProces->terminate();

    if (outputLine.contains("cannot rename"))
        return false;
    return true;
}

void Phone::adbPowerOff()
{
    adbProces->start("\"" + adb + "\" shell shutdown");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
}

void Phone::adbReboot()
{
    adbProces->start("\""+ adb + "\" shell reboot");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
}

void Phone::adbRebootBootloader()
{
    adbProces->start("\""+ adb + "\" shell reboot bootloader");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
}

void Phone::adbRebootRecovery()
{
    adbProces->start("\"" + adb + "\" shell reboot recovery");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
}

void Phone::fastbootPowerOff()
{
    adbProces->start("\""+fastboot+"\" oem powerdown");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

void Phone::fastbootReboot()
{
    adbProces->start("\""+fastboot+"\" reboot");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

void Phone::fastbootRebootBootloader()
{
    adbProces->start("\""+fastboot+"\" reboot-bootloader");
    adbProces->waitForFinished(-1);
    adbProces->terminate();
    this->slotConnectionChanged(FASTBOOT, this->serialNumber);
}

QStringList Phone::getGoogleAccounts()
{
    QString output;
    QSettings settings;
//    QString operation = "\""+sdk+"\""+ "adb shell su -c 'busybox grep gmail-ls /data/system/sync/accounts.xml'";
//su -c 'busybox grep gmail-ls /data/system/sync/accounts.xml'
    adbProces->start("\""+adb+"\" shell " + busybox + " grep gmail-ls /data/system/sync/accounts.xml");
    adbProces->waitForFinished(-1);
    output = adbProces->readAll();
    qDebug()<<"Phone::getGoogleAccounts(): "<<output;
    if (output.contains("permission") && !output.contains("account"))//////////////////////////////zmeinic
    {
        adbProces->start( "\""+adb+"\" shell su -c '" + busybox + " grep gmail-ls /data/system/sync/accounts.xml'");
        adbProces->waitForFinished(-1);
        output = adbProces->readAll();
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
    QSettings settings;
    adbProces->start("\"" + adb + "\" shell" + busybox + " arp");
    QString tmp;
    adbProces->waitForFinished(-1);
    tmp = adbProces->readAll();
    qDebug()<<"Phone::getIP(): "<<tmp.toStdString().c_str();

    if (tmp.length() < 5)
    {
        qDebug()<<"Phone::getIP(): wlan is off";
        return "";
    }
    tmp.remove(0,tmp.lastIndexOf(" "));
    tmp.remove("\n");

    adbProces->start("\"" + adb + "\" shell" + busybox + " ifconfig "+tmp);
    adbProces->waitForFinished(-1);
    tmp = adbProces->readAll();
    tmp.remove(QRegExp(".*inet addr:"));
    tmp.remove(QRegExp("\\s.*"));

    qDebug()<<"Phone::getIP(), gotIP:  "<<tmp.toStdString().c_str();
    return tmp;
}
