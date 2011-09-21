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


#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include "../classes/phone.h"
#include "../classes/computer.h"
#include "../dialogs/appinfo.h"
#include "../dialogs/dialogkopiuj.h"
#include "../dialogs/appdialog.h"
#include "../classes/models/filetablemodel.h"
#include "../classes/mytableview.h"
#include "settingswidget.h"
#include <QtGui>

namespace Ui {
    class FileWidget;
}

class ThreadFind : public QThread
{
    Q_OBJECT
public:
    void run();
    QString path;
    QString fileName;
    QString sdk;

signals:
    void foundFile(File);
    void finished();
};

class FileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileWidget(QWidget *parent = 0, SettingsWidget * settings = NULL);
    ~FileWidget();
    static QString humanReadableSize(QString size);
    static bool unpack(QString inFile,QString outPath,QString fileToUnpack,QString outName);
    Phone *phone;
    Computer *computer;
    SettingsWidget *settings;

    FileTableModel *phoneModel;
    FileTableModel *phoneLeftModel;
    FileTableModel *computerModel;
    FileSortModel *phoneSortModel;
    FileSortModel *phoneLeftSortModel;
    FileSortModel *computerSortModel;
    FileTableModel *findModel;

    MyTableView *rightTableView;
    MyTableView *leftTableView;
protected:
    void changeEvent(QEvent *e);
    virtual void keyPressEvent( QKeyEvent *e );
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
private:
    QString leftMode;
    appInfo *appInfoDialog;
    bool dialogKopiujShowModal;
    dialogKopiuj *dialog;
    appDialog *appsDialog;
    Ui::FileWidget *ui;

    ThreadFind threadFind;
    bool alwaysCloseCopy;
    QAction *actionAppInfo;

    QList<File> *computerFilesToCopy(QList<File> * fileList);
    QList<File> *phoneFilesToCopy(QList<File> * fileList, Phone *phone);

    QString renameOldName;
    bool rightChangeName;
    bool rightNewDir;
    QString sdk;

    Phone *phoneLeft;
    QTabBar *rightTabBar;
    QTabBar *leftTabBar;

    bool processShellIsRunning;
    bool leftChangeName;
    bool leftNewDir;

    QMenu *computerMenu, *phoneLeftMenu, *phoneRightMenu;


    QString oldComputerPath;

public slots:
    void rightDisplay();
    void leftDisplay();
    static App * getAppInfo(QString filePath);

private slots:
    void on_pushButton_pressed();
    void foundFile(File);
    void findFinished();
    void on_toolButtonFind_pressed();
    void showAppInfo();

    void setLeftFilter(QString);
    void setRightFilter(QString);

    void leftDoubleClick();
    void rightDoubleClick();
    void leftComboBoxScroll();
    void rightComboBox();
    void leftComboBox();
    void leftTableWidgetActivated();
    void rightTableWidgetActivated();

    void connectionChanged();

    void on_leftPushButtonPhoneComputerSwitch_pressed();
    void on_pushButtonHiddenFiles_pressed();
    void on_pushButtonDelete_pressed();
    void on_pushButtonRefresh_pressed();
    void on_pushButtonNewDir_pressed();
    void on_pushButtonCopy_pressed();
    void on_pushButtonSelectAll_pressed();
    void on_pushButtonRename_pressed();

    void on_rightPushButtonFolderUp_pressed();
    void on_leftPushButtonFolderUp_pressed();

    void computerRename();
    void computerCopy();
    void computerNewDir();
    void computerDelete();
    void computerHiddenFiles();
    void computerRenameSlot(QModelIndex indexS, QModelIndex indexE);
    void computerDisplay(QTableWidget*);
    void computerContextMenu(const QPoint&,QTableView*);
    void installAppFromComputer();

    void rightRefresh();
    void rightSelectAll();
    void rightSelectedCount();
    void rightContextMenu(const QPoint&);

    void leftSelectAll();
    void leftRefresh();
    void leftSelectedCount();
    void leftContextMenu(const QPoint&);

    void phoneGoToDir();
    void phoneRename();
    void phoneCopy();
    void phoneNewDir();
    void phoneDelete();
    void phoneHiddenFiles();
    void phoneRenameSlot(QModelIndex indexS, QModelIndex indexE);
    void phoneDisplay(QTableView*);
    void phoneContextMenu(const QPoint&,QTableView*);

    void phoneOpenInNewTab();
    void rightTabBarPageChanged();
    void rightTabBarCloseTab(int);

    void computerOpenInNewTab();
    void leftTabBarPageChanged();
    void leftTabBarCloseTab(int);

    void copySlotToComputer(QStringList list);
    void copySlotToPhone(QStringList list);
    void copySlotToPhoneLeft(QStringList list);
signals:
    void phoneConnectionChanged(int);
    void progressValue(int value, int max);
    void copyFinished(int);
};

#endif // FILEWIDGET_H
