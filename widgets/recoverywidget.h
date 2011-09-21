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


#ifndef RECOVERYWIDGET_H
#define RECOVERYWIDGET_H

#include <QtGui>
#include "../dialogs/dialogkopiuj.h"
#include "../classes/phone.h"


namespace Ui {
    class RecoveryWidget;
}

class RecoveryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecoveryWidget(QWidget *parent = 0);
    ~RecoveryWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecoveryWidget *ui;

    QString nandroidScript;
    QProcess procesNandroid;

    bool dialogKopiujShowModal;
    bool alwaysCloseCopy;
    dialogKopiuj *dialog;

    QString romFileName;
    QString sdk;

    bool recoverySDmounted;

private slots:
    void wipeBattery();
    void mountSDcard();
    void flashZip();
    void flashZipCopied();
    void flashZipReboot();
    void fixUID();
    void fixUIDoutput();
    void fixUIDfinished();
    void wipeData();
    void wipeDataFinished();

    void nandroidBackup();
    void nandroidBackupEdit();
    void nandroidBackupOutput();
    void nandroidBackupPage();
    void nandroidBackupFinished();
    void nandroidRestore();
    void nandroidRestoreCombo();
    void nandroidRestoreOutput();
    void nandroidRestorePage();
    void nandroidRestoreFinished();
    void nandroidRestoreFound();
};

#endif // RECOVERYWIDGET_H
