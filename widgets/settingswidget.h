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


#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QtGui>
#include "../dialogs/appdialog.h"
#include "../classes/animation.h"
#include "../classes/models/apptablemodel.h"
#include "../classes/models/backuptablemodel.h"
#include "../classes/models/filetablemodel.h"


namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    bool alwaysCloseCopy;
    bool phoneHiddenFiles;
    bool computerHiddenFiles;
    bool saveWindowPosition;
    bool rememberComputerPath;
    bool rememberPhonePath;
    bool showComputerColumnsHeaders;
    bool showPhoneColumnsHeaders;
    bool dialogKopiujShowModal;
    bool killDemonOnExit;
    bool checkForUpdatesOnStart;
    bool color;
    bool colorShellFiles;
    bool showAppIcon;
    bool showAppName;
    bool clearSettings;
    bool enableAnimations;
    bool getQR;
    bool getCyrketVer;
    bool showCopyConfirmation;

    QString phonePath;
    QString computerPath;
    QString showAppNameConfig;
    QString sdkPath;
    QString wlanIP;
    QString sdFolder;

    int sortByColumn;
    int getAppInfo;
    int getSystemAppInfo;
    int getBackupInfo;
    int BackupDefaultAction;
    int RestoreDefaultAction;
    int animationCurve;
    int animationDuration;
    int animationDirection;
    int layoutDirection;

    QFont fontApp;
    QFont fontShell;
    QFont fontTables;

    QByteArray windowGeometry;

    void setSettings();

    int currentIndex;
    QStandardItemModel *appsColumnModel;
    QStringList appColumnList;
    QStandardItemModel *backupsColumnModel;
    QStringList backupColumnList;
    QStandardItemModel *computerColumnModel;
    QStringList computerColumnList;
    QStandardItemModel *phoneColumnModel;
    QStringList phoneColumnList;

    QColor shellBackgroundColor;
    QColor shellFontColor;

    QSize toolBarIconSize;
    Qt::ToolButtonStyle toolBarStyle;
    bool toolBarHideButton;
    Qt::ToolBarArea toolBarArea;
protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingsWidget *ui;
    void createCurveIcons();
    Animation animation;
    void setBackgroundColor(QLabel *label, QColor color);

public slots:
    void saveSettings();
    void changeFont();

private slots:
    void on_pushButtonChangeSDKPath_pressed();
    void getSettings();
    void settingsSlotChanged();
    void chooseFontApp();
    void chooseFontTables();
    void chooseFontShell();
    void setSettingsToDefaults();
    void on_buttonRemoveSettings_pressed();
    void curveChanged(int);

    void changePage(int);
    void animationFinished();

    void on_buttonAssociate_clicked();

    void on_shellPushButtonBackground_clicked();

    void on_shellPushButtonFont_clicked();
    void detectSdExtFolder();

signals:
    void settingsChanged();
};

#endif // SETTINGSWIDGET_H
