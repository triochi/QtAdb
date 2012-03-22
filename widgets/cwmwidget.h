/***********************************************************************
*Copyright 2010-20XX by nijel
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
*   @author nijel (nijel8@gmail.com)
*
************************************************************************/


#ifndef CWMWIDGET_H
#define CWMWIDGET_H

#include "../classes/phone.h"
#include "../classes/computer.h"
#include "../classes/models/filetablemodel.h"
#include "../classes/mytableview.h"
#include "settingswidget.h"
#include "../dialogs/fipdialog.h"
#include <QtGui>
#include <QThread>
#include <QTextCodec>



#include "../dialogs/dialogkopiuj.h"


namespace Ui {
    class CwmWidget;
}

class ThreadSdcard : public QThread
{
    Q_OBJECT
public:
    void run();
    QString sdk, backupPath;
    static void sleep(unsigned long secs){QThread::sleep(secs);}
signals:
    void backupVerifiable();
};

class CwmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CwmWidget(QWidget *parent = 0);
    ~CwmWidget();
    static QString humanReadableSize(QString size);
    Phone *phone;
    Computer *computer;
    SettingsWidget *settings;
    FileTableModel *phoneModel;
    FileSortModel *phoneSortModel;
    MyTableView *sdcardTableView;
    QTextCodec *codec;

protected:
   // void mousePressEvent(QMouseEvent *e);

private:
    Ui::CwmWidget *ui;
    QString renameOldName;
    bool sdcardChangeName;
    bool NewDir;
    QString sdk, which, commandRunning, readLog, backupPath, currentDir;
    bool processShellIsRunning;
    QMenu *phoneMenu;
    QProcess *process;
    QProcess *processFind;
    QProcess *processWhich;
    QProcess *proc;
    ThreadSdcard *threadSdcard;
    QList<File> *getFileList();


    dialogKopiuj *dialog;


public slots:
    void sdcardDisplay();

private slots:
    void propsDialog();
    void sdcardDoubleClick();
    void sdcardLineEdit();
    void connectionChanged();
    void on_pushButtonDirectoryUp_pressed();
    void phoneRename();
    void phoneNewDir();
    void phoneDelete();
    void phoneHiddenFiles();
    void phoneRenameSlot(QModelIndex indexS, QModelIndex indexE);
    void phoneDisplay(QTableView*);
    void phoneContextMenu(const QPoint&);
    void on_buttonFixPermissions_pressed();
    void printProcess(QString processOut);
    void processFinished();
    void buttonsEnabled();
    void buttonsDisabled();
    QString adbPushTool(QString toolName);
    void on_buttonCreate_pressed();
    void extendedcommandFile(QString infoLine, QString commandLine);
    void tailLog();
    void readFromLog();
    void readFromProcess();
    void processStarted();
    void on_buttonSdinfo_pressed();
    void on_buttonWipe_pressed();
    void on_buttonInstall_pressed();
    void on_buttonInsert_pressed();
    void activateButtonInsert();
    void on_buttonBackup_pressed();
    void md5sumVerify();
    void on_buttonRestore_pressed();
    void backupAvailable();
    void on_buttonUpdate_pressed();
    void checkFlash();
    void on_buttonFlash_pressed();
    void finishedWhich();
    void readFromProcessWhich();
    void createPartition();
    void on_buttonFixMarket_pressed();
    void on_buttonRecovery_pressed();
    void sdrestoreCanceled();
    void sdbackupCanceled();
    void connectSignals();
    void disconnectSignals();
    void setTabCreate(int i);
    void setTabFix(int i);
    void mountsEnable();
    void mountsDisable();
    void mountsUpdate();
    void on_buttonCache_pressed();
    void on_buttonData_pressed();
    void on_buttonSystem_pressed();
    void on_buttonSdext_pressed();
    void on_buttonSdcard_pressed();

signals:
    void phoneConnectionChanged(int);
    void progressValue(int value, int max);
    void copyFinished(int);
    void copyCanceled();
};

#endif // CWMWIDGET_H
