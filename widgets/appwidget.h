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


#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QtGui>

#include "../classes/phone.h"
#include "../classes/computer.h"
#include "../dialogs/appinfo.h"
#include "../dialogs/appdialog.h"
#include "../dialogs/dialogkopiuj.h"
#include "../widgets/filewidget.h" //potrzebny TableWidgetItem
#include "../classes/models/apptablemodel.h"
#include "../classes/models/backuptablemodel.h"

namespace Ui
{
    class AppWidget;
}


class ThreadApps : public QThread
{
    Q_OBJECT
public:
    bool systemApps;
    void run();
    QString sdk;
    QList<App> appList;

signals:
    void gotApp(App);
    void maximum(int);
    void value(int);
    void progressValue(int value, int max);
    void gotAllApps(QThread *);
    void missingAapt();
};


class ThreadBackups : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk;

signals:
    void gotBackup(Backup);
    void maximum(int);
    void value(int);
    void progressValue(int value, int max);
    void gotAllApps(QThread *);
};



class AppWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppWidget(QWidget *parent = 0);
    ~AppWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AppWidget *ui;

    bool alwaysCloseCopy;
    bool dialogKopiujShowModal;
    dialogKopiuj *dialog;
    QMenu *appMenu, *backupMenu;
    ThreadApps threadApps;
    ThreadBackups threadBackups;
    appDialog *appsDialog;
//    QList<App> appList;
    App * getAppInfo(QString filePath);
//    QList<Backup> backups;
//    QList<App> apps;
//    QList<App> systemApps;
    bool firstGetApps;//, firstGetBackups;
    QString sdk;

    AppTableModel *appModel;
    AppSortModel *appSortModel;
    AppTableModel *systemAppModel;
    AppSortModel *systemAppSortModel;
    BackupTableModel *backupModel;
    BackupSortModel *backupSortModel;

    bool withData;
    bool withApk;
    Phone *phone;

public slots:

    void openMarket();
    void openAppBrain();
    void getCyrketVersions();
    void insertApp(App app);
    void insertBackup(Backup backup);
    void gotAllApps(QThread *);
    void appsSelectedCount();
    void appsContextMenu(const QPoint&);
    void backupsContextMenu(const QPoint&);
    void copyAppToPC();

private slots:
    void getAppInfo();

    void filter();
    void missingAapt();
    static bool unpack(QString inFile,QString outPath,QString fileToUnpack,QString outName);
    static quint32 qbytearrayToInt32(QByteArray array);
    static quint16 qbytearrayToInt16(QByteArray array);
    void on_toolButtonRemoveBackup_pressed();
    void on_toolButtonBackup_pressed();
    void on_toolButtonRestore_pressed();
    void on_toolButtonUninstall_pressed();
    void on_toolButtonInstall_pressed();
    void toolButtonBackupApp();
    void toolButtonBackupData();
    void toolButtonBackupAppAndData();
    void toolButtonRestoreApp();
    void toolButtonRestoreData();
    void toolButtonRestoreAppAndData();
    void toolButtonInstallOnSd();
    void comboBoxAppsChanged();
    void refreshApps();
signals:
    void progressValue(int value, int max);
    void progressFinished();
};
#endif // APPWIDGET_H
