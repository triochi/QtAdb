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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QCryptographicHash>

#include "../classes/phone.h"
#include "../classes/computer.h"
#include "logcatdialog.h"
#include "../classes/ecwin7.h"

#include <stdio.h>
#include <iostream>

#include <QtGui>

#include "dialogkopiuj.h"
#include "../threads/screenshotthread.h"
#include "connectWifi.h"
#include "aboutdialog.h"
#include "appdialog.h"
#include "../classes/updateapp.h"
#include "appinfo.h"
#include "../widgets/settingswidget.h"
#include "../widgets/filewidget.h"
#include "../widgets/shellwidget.h"
#include "../widgets/screenshotwidget.h"
#include "../widgets/phoneinfowidget.h"
#include "../widgets/messagewidget.h"
#include "../widgets/appwidget.h"
#include "../widgets/recoverywidget.h"
#include "../classes/animation.h"
#include "../widgets/fastbootwidget.h"
#include "registerdialog.h"

using namespace std;

namespace Ui
{
    class MainWindow;
}

class Action{
public:
    enum Flags{
        Disconnected = 1,
        Device = 2,
        Recovery = 4,
        Fastboot = 8
    };

    QToolButton * button;
    QAction * actionMenu;
    QAction * actionToolBar;
    int flags;
    bool visible;
    QString text;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *object, QEvent *event);
    virtual void keyPressEvent( QKeyEvent *e );

    void mousePressEvent(QMouseEvent*);
#ifdef WIN7PROGRESS
    virtual bool winEvent(MSG *message, long *result);
#endif
private:
    Ui::MainWindow *ui;
    EcWin7 win7;

//dialogs
    ConnectWifi *connectWifiDialog;
    AboutDialog aboutDialog;
    LogcatDialog *logcatDialog;

    UpdateApp updateApp;
//widgets
    SettingsWidget *settingsWidget;
    FileWidget *fileWidget;
    ShellWidget *shellWidget;
    ScreenshotWidget *screenshotWidget;
    PhoneInfoWidget *phoneInfoWidget;
    MessageWidget *messageWidget;
    AppWidget *appWidget;
    RecoveryWidget *recoveryWidget;
    FastbootWidget *fastbootWidget;

    QWidget *currentWidget;
    QWidget *targetWidget;

    bool debugMode;
    QFile *debugFile;
    bool killDemonOnExit;

    bool showNoUpdates;

    QString ipAddress, portNumber;

    QTextCodec *codec;

    Animation animation;

    QSystemTrayIcon *systemTray;

    QList<Action> akcje;
    QToolButton * buttonPageInfo;
    QAction * actionPageInfo;

    bool verifyKey(QString email,QString key);
    bool verifyRegistered(QString email);

public slots:

    void refreshState();

private slots:

    void phoneConnectionChanged(int);
    //apps

    void on_actionO_programie_triggered();
    void showPageMessages();
    void showPageContacts();
    void showPageApps();
    void showPageFiles();
    void showPageRecovery();
    void showPageFastboot();
    void showPagePhoneInfo();
    void showPageScreenshot();
    void showPageSettings();
    void showPageShell();
    void showPageDisconnected();

    void animationFinished();
    void startAnimation(QWidget *target);

//wifi/usb
    void restartInWifi();
    void restartInUsb();
    void connectWifi();

    void setLanguageDir(QAction* action);
    void setLanguageRes(QAction* action);
    void fillLanguages();

    //updates
    void updatesCheck();
    void updatesCheckFinished(bool, QString, QString);

    void showLogcat();
    void smsReceived(QString number, QString body);
    void smsResult(QString result);

    void systemTrayActivated(QSystemTrayIcon::ActivationReason);

    void addButton(QIcon icon, QString textTr, QString text, const char * method, int flags);
    void changeToolBar();
    void setButtonDown(int number);
    void disableActions(Action::Flags);
    void setProgressValue(int value, int max);
    void setProgressDisable();

    void donateMessage();
    void on_actionEnter_register_key_triggered();
};

#endif // MAINWINDOW_H
