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


#ifndef DIALOGKOPIUJ_H
#define DIALOGKOPIUJ_H

#include <QDialog>
#include <QStringList>
#include <QTableWidgetItem>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <QMessageBox>
#include <QCloseEvent>

#include "../classes/models/filetablemodel.h"
#include "../classes/models/apptablemodel.h"
#include "../classes/phone.h"
#include "../classes/computer.h"

namespace Ui {
    class dialogKopiuj;
}

class ThreadCopy : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    QString sourcePath;
    QString targetPath;
    QList<File> *fileList;
    QList<App> *appList;
    int mode;
    QProcess *proces;

signals:
    void copied();
    void nextFile(QString fileName, QString pathFrom, QString pathTo, int fileSize, int counter);
};

class ThreadProgress : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    QString filePath;
    int mode;
    int maxSize;

signals:
    void progressValue(int fileSize);
};

class dialogKopiuj : public QDialog {
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);

public:
    enum Mode{
        PhoneToComputer = 0,
        PhoneToPhone = 1,
        ComputerToPhone = 2,
        AppsToComputer = 3
    };

    dialogKopiuj(QWidget *parent, QList<File> * fileList, QString sdk, int, QString sourcePath, QString targetPath);
    dialogKopiuj(QWidget *parent, QList<App> * appList, QString sdk, int, QString targetPath);
    ~dialogKopiuj();

    QList<QTableWidgetItem*> lista;
    QString sciezkaKomputer,sciezkaTelefon;
    static void fileRemove(QString filePath,int mode);

private:
    Ui::dialogKopiuj *ui;

    QString sourcePath, targetPath;
    ThreadCopy *threadCopy;
    ThreadProgress *threadProgress;
    bool opFinished;
    int mode;
    int filesCopiedSize;
    int remains;

    QString humanReadableSize(QString size);

public slots:
    void closeAfterFinished();
    void copied();
    void nextFile(QString fileName, QString pathFrom, QString pathTo, int fileSize, int counter);
    void setProgressValue(int value);
signals:
    void progressValue(int value, int max);
};

#endif // DIALOGKOPIUJ_H
