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


#include "computer.h"
#include <QDateTime>

Computer::Computer()
{
    this->procesEvents=false;
}

bool Computer::cd(QString dir)
{
    if (this->directory.cd(dir))
        return true;
    return false;
}

bool Computer::cdUp()
{
    if (this->directory.cdUp())
        return true;
    return false;
}

QList<File> *Computer::getFileList()
{
    QDir directory(this->getPath());
    QFileInfoList fileInfoList;
    if (this->hiddenFiles)
    {
        fileInfoList = directory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    }
    else
    {
        fileInfoList = directory.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    }
    QFileInfo fileInfo;
    File file;
    QList<File> *fileList = new QList<File>;
    QFileIconProvider *provider = new QFileIconProvider;
    while (!fileInfoList.isEmpty())
    {
        if (this->procesEvents)
            qApp->processEvents();
        fileInfo = fileInfoList.takeFirst();

        file.fileIcon = provider->icon(fileInfo);
        file.fileName = fileInfo.fileName();
        file.fileSize = QString::number(fileInfo.size());
        file.fileDate = fileInfo.lastModified().toString("MMM dd yyyy");
        file.filePath = fileInfo.absoluteFilePath();
        file.filePermissions = "";
        file.fileOwner = fileInfo.owner();
        if (fileInfo.isDir())
            file.fileType = "dir";
        else
            file.fileType = "file";

        fileList->append(file);
    }
    delete provider;
    return fileList;
}

FileList *Computer::getFileList(QString filter)
{
    FileList *fileList = new FileList;
    QFileInfoList list = this->directory.entryInfoList();
    QFileIconProvider *provider;

    QStringList date;
    QString tmp;
    while (!list.isEmpty())
    {
        date=list.first().lastModified().toString().split(" ");
        tmp=date.at(2)+" "+date.at(1)+" "+date.at(4);
        tmp.remove(".");
        fileList->date.append(tmp);
        fileList->name.append(list.first().fileName());
        fileList->size.append(QString::number(list.first().size()));
        provider=new QFileIconProvider;
        fileList->icon.append(provider->icon(list.first()));
        if (list.first().isDir())
            fileList->type.append("dir");
        else
        {
            if (!fileList->name.last().contains(filter))
            {
                fileList->date.removeLast();
                fileList->name.removeLast();
                fileList->size.removeLast();
                fileList->icon.removeLast();
            }
            fileList->type.append("file");
        }
        delete provider;
        list.removeFirst();
    }
    if (fileList->name.length()==0)
        return  fileList;
    //usun . i ..
    if (fileList->name.count()>0)
    {
//        if (fileList.name.first()==".")
//        {
        int x=fileList->name.indexOf(".");
        if (x!=-1)
        {
            fileList->date.removeAt(x);
            fileList->name.removeAt(x);
            fileList->size.removeAt(x);
            fileList->type.removeAt(x);
            fileList->icon.removeAt(x);
        }

        x=fileList->name.indexOf("..");
        if (x!=-1)
        {
            fileList->date.removeAt(x);
            fileList->name.removeAt(x);
            fileList->size.removeAt(x);
            fileList->type.removeAt(x);
            fileList->icon.removeAt(x);
        }
    }
    return fileList;
}

bool Computer::getHiddenFilesState()
{
    return this->hiddenFiles;
}

QString Computer::getPath()
{
    if (this->directory.path().endsWith("/"))
        return this->directory.path();
    else
        return this->directory.path() + "/";
}

bool Computer::makeDir(QString newDir)
{
    if (this->directory.mkdir(newDir))
        return true;
    return false;
}

bool Computer::remove(File file)
{
    if (file.fileType == "dir")
    {
        this->deleteDir(file.filePath);
    }
    else
    {
        this->deleteFile(file.filePath);
    }
    return false;
}

bool Computer::rename(QString oldName, QString newName)
{
    if (this->directory.rename(oldName,newName))
        return true;
    return false;
}

void Computer::setHiddenFiles(bool hiddenFiles)
{
    this->hiddenFiles=hiddenFiles;
    if (this->hiddenFiles)
    {
        this->directory.setFilter(QDir::Files | QDir::NoSymLinks |QDir::Dirs | QDir::Hidden);
        this->directory.setSorting(QDir::Name | QDir::IgnoreCase | QDir::DirsFirst);
    }
    else
    {
        this->directory.setFilter(QDir::Files | QDir::NoSymLinks |QDir::Dirs);
        this->directory.setSorting(QDir::Name | QDir::IgnoreCase | QDir::DirsFirst);
    }
}

void Computer::setPath(QString newPath)
{
    this->directory.setPath(newPath);
}

bool Computer::deleteFile(QString fileName)
{
    if (fileName.size() < 1) {
        return true;
    }

    QFile file(fileName);

    return file.remove();
}

bool Computer::deleteDir(QString dirName)
{
    QDir directory(dirName);

    if (!directory.exists())
    {
        return true;
    }

    QStringList dirs = directory.entryList(QDir::Dirs | QDir::Hidden);
    QStringList files = directory.entryList(QDir::Files | QDir::Hidden);

    QList<QString>::iterator f = files.begin();
    QList<QString>::iterator d = dirs.begin();

    bool error = false;

    for (; f != files.end(); ++f) {
        if (!deleteFile(directory.path() + '/' + (*f))) {
            error = true;
        }
    }

    for (; d != dirs.end(); ++d) {
        if ((*d) == "." || (*d) == "..") {
            continue;
        }

        if (!deleteDir(directory.path() + '/' + (*d))) {
            error = true;
        }
    }

    if(!directory.rmdir(directory.path())) {
        error = true;
    }

    return !error;

}
