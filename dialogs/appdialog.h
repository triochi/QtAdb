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


#ifndef APPDIALOG_H
#define APPDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QTimer>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QCloseEvent>
#include <QSettings>

#include "../classes/phone.h"
#include "../classes/models/apptablemodel.h"

class ThreadBackup : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    bool withData;
    bool withApk;
    QList<App> appList;
signals:
    void backedUp(QString, QString);
    void nextApp(App);
};

class ThreadRestore : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    bool withData;
    bool withApk;
    QList<App> appList;
signals:
    void restored(QString, QString);
    void nextApp(App);
};

class ThreadInstall : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    QList<App> appList;
    bool reinstall;
signals:
    void installed(QString, QString);
    void nextApp(App);
};

class ThreadUninstall : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;
    QList<App> appList;
    bool system;
    bool keepData;
signals:
    void uninstalled(QString, QString);
    void nextApp(App);
};

namespace Ui {
    class appDialog;
}

class appDialog : public QDialog
{
    Q_OBJECT

public:
    enum Operation{
        Install = 0,
        Uninstall = 1,
        Backup = 2,
        Restore = 3,
        Reinstall = 4
    };
    enum Mode{
        Application = 0,
        Data = 1,
        AppAndData = 2,
        None = 3
    };

    appDialog(QWidget *parent,QList<App> appList,int operation,int mode);
    appDialog(QList<App> appList,int operation,int mode);
    ~appDialog();

protected:
    void changeEvent(QEvent *e);
    void closeEvent (QCloseEvent *);

private:
    QProcess *proces;
    QTimer *timer;
    QTimer *clock;
    Ui::appDialog *ui;
    QList<App> appList;
    int mode, operation;
    QString sdk;
    int licznik;
    ThreadBackup *threadBackup;
    ThreadRestore *threadRestore;
    ThreadInstall *threadInstall;
    ThreadUninstall *threadUninstall;
    bool end;
public:
    bool system;

private slots:
    void install();
    void reinstall();
    void uninstall();
//    void uninstallFin();
//    void installFin();
    void backup();
//    void backupFin(QString);
    void restore();
//    void restoreFin(QString);
    void finished(QString, QString);
    void clockTimeout();
    void nextApp(App);
signals:
    void closed();
    void progressValue(int value, int max);

};

#endif // APPDIALOG_H
