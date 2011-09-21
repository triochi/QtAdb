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


#ifndef COMPUTER_H
#define COMPUTER_H

#include <QObject>
#include <QDir>
#include "phone.h"
#include <QFileInfoList>
#include <QFileIconProvider>
#include "./models/filetablemodel.h"

class Computer: public QObject
{
    Q_OBJECT
public:
    Computer();
    bool cd(QString dir);
    bool cdUp();
    QList<File> *getFileList();
    FileList *getFileList(QString filter);
    bool getHiddenFilesState();
    QString getPath();
    bool makeDir(QString newDir);
    bool remove(File file);
    bool deleteFile(QString fileName);
    bool deleteDir(QString dirName);
    bool rename(QString oldName,QString newName);
    void setHiddenFiles(bool);
    void setPath(QString newPath);
    bool procesEvents;

private:
//    QString path;

    QDir directory;
    bool hiddenFiles;
};

#endif // COMPUTER_H
