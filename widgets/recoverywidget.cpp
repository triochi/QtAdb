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


#include "recoverywidget.h"
#include "ui_recoverywidget.h"

RecoveryWidget::RecoveryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecoveryWidget)
{
    ui->setupUi(this);

    this->setLayout(this->ui->layoutRecovery);

    QSettings settings;
    this->sdk=settings.value("sdkPath").toString();
    this->dialog = NULL;
    this->recoverySDmounted=false;

    QProcess *nandroid = new QProcess;
    QString output, tmp;
    QStringList tmpLines;
    nandroid->start("\"" + sdk + "\"adb shell find /sbin -name nandroid-mobile*.sh");
    nandroid->waitForFinished(-1);
    output = nandroid->readAll();
    tmpLines = output.split("\n", QString::SkipEmptyParts);
    this->nandroidScript.clear();
    while (tmpLines.size() > 0)
    {
        tmp = tmpLines.takeFirst();
        if (tmp.contains("find"))
        {
            continue;
        }
        else
        {
            this->nandroidScript = tmp;
        }
    }

    if (this->nandroidScript.isEmpty())
    {
        this->ui->buttonNandroidBackup->setDisabled(true);
        this->ui->buttonNandroidRestore->setDisabled(true);
    }
    else
    {
        this->ui->buttonNandroidBackup->setDisabled(false);
        this->ui->buttonNandroidRestore->setDisabled(false);
    }

    QProcess *process=new QProcess();

    process->start("\""+sdk+"\"adb shell ls /sbin/wipe");
    process->waitForFinished(-1);
    tmp = process->readAll();
    if (tmp.contains("No such file"))
        this->ui->buttonClearBattery->setDisabled(true);
    else
        this->ui->buttonClearBattery->setEnabled(true);
    process->terminate();
    delete process;
    process=new QProcess();
    process->start("\""+sdk+"\"adb shell ls /sbin/ums_toggle");
    process->waitForFinished(-1);
    tmp = process->readAll();
    if (tmp.contains("No such file"))
        this->ui->buttonMountSD->setDisabled(true);
    else
        this->ui->buttonMountSD->setEnabled(true);
    process->terminate();
    delete process;

    process=new QProcess();
    process->start("\""+sdk+"\"adb shell ls /sbin/fix_permissions");
    process->waitForFinished(-1);
    tmp = process->readAll();
    if (tmp.contains("No such file"))
        this->ui->buttonFixUID->setDisabled(true);
    else
        this->ui->buttonFixUID->setEnabled(true);
    process->terminate();
    delete process;

    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageRecoveryStart);
    ui->pageWipeData->setLayout(ui->layoutWipeData);
    ui->pageFixUID->setLayout(ui->layoutFixUID);
    ui->pageFlashZIP->setLayout(ui->layoutFlashZIP);
    ui->pageNandroidBackup->setLayout(ui->layoutNandroidBackup);
    ui->pageNandroidRestore->setLayout(ui->layoutNandroidRestore);
    ui->pageRecoveryStart->setLayout(ui->layoutRecoveryStart);


    connect(this->ui->buttonClearBattery, SIGNAL(clicked()), this, SLOT(wipeBattery()));
    connect(this->ui->buttonFixUID, SIGNAL(clicked()), this, SLOT(fixUID()));
    connect(this->ui->buttonMountSD, SIGNAL(clicked()), this, SLOT(mountSDcard()));
    connect(this->ui->buttonNandroidBackup, SIGNAL(clicked()), this, SLOT(nandroidBackupPage()));
    connect(this->ui->buttonNandroidRestore, SIGNAL(clicked()), this, SLOT(nandroidRestorePage()));
    connect(this->ui->checkNandroidBackupPath, SIGNAL(toggled(bool)), this, SLOT(nandroidBackupEdit()));
    connect(this->ui->checkNandroidBackupString, SIGNAL(toggled(bool)), this, SLOT(nandroidBackupEdit()));
    connect(this->ui->buttonNandroidBackupStart, SIGNAL(clicked()), this, SLOT(nandroidBackup()));
    connect(this->ui->buttonNandroidRestoreStart, SIGNAL(clicked()), this, SLOT(nandroidRestore()));
    connect(this->ui->comboNandroidRestore, SIGNAL(currentIndexChanged(QString)), this, SLOT(nandroidRestoreCombo()));
    connect(this->ui->buttonWipeData, SIGNAL(clicked()), this, SLOT(wipeData()));
}

RecoveryWidget::~RecoveryWidget()
{
    delete ui;
}

void RecoveryWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RecoveryWidget::fixUID()
{
    this->procesNandroid.setProcessChannelMode(QProcess::MergedChannels);
    this->procesNandroid.start("\"" + sdk + "\"adb shell fix_permissions");
    connect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(fixUIDoutput()));
    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(fixUIDfinished()));
    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageFixUID);
}

void RecoveryWidget::fixUIDfinished()
{
    this->procesNandroid.terminate();
    disconnect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(fixUIDoutput()));
    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(fixUIDfinished()));
    QMessageBox::information(this, tr("Fix UID finished!"), tr("Fixing UID mismatches is finished."), QMessageBox::Ok);
}

void RecoveryWidget::fixUIDoutput()
{
    QString output;
    output = this->procesNandroid.readAll();
    int i;
    for (i = 0; i < output.length(); i++)
    {
        if (output.at(i).unicode() == 13)
            output[i] = ' ';
        if (output.at(i).unicode() == 10)
            output[i] = '\n';
    }
    this->ui->textFixUID->append(output);
}

void RecoveryWidget::flashZip()
{
    if (QMessageBox::question(this, tr("Flash Zip"), tr("Are you sure??"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;
    this->romFileName = QFileDialog::getOpenFileName(this, tr("Open ROM File"), "/", tr("Zip files (*.zip)"));
    QFileInfo fInfo = QFileInfo(this->romFileName);
    FileList *fileList = new FileList;
    fileList->name.append(this->romFileName);
    fileList->size.append(QString::number(fInfo.size()));
    fileList->date.append(fInfo.lastModified().toString());
    fileList->type.append("file");
    this->romFileName = fInfo.fileName();

    QProcess tmp;
    tmp.start("\"" + sdk + "\"adb shell mount /sdcard");
    tmp.waitForFinished(-1);
    tmp.terminate();
//    if (this->dialog != NULL)
//        delete this->dialog;
//    this->dialog = new dialogKopiuj(this, fileList, this->sdk, dialogKopiuj::ComputerToPhone, fInfo.absolutePath(), "/sdcard/");
//    this->dialog=new dialogKopiuj(this,fileList,this->sdk,"computerToPhone",this->phone,NULL,this->computer);
//    if (this->alwaysCloseCopy)
//        this->dialog->closeAfterFinished();
//    if (this->dialogKopiujShowModal)
//        this->dialog->setModal(true);
//    this->dialog->show();


//    connect(this->dialog,SIGNAL(finished(int)),this,SLOT(flashZipCopied()));

    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageFlashZIP);
}

void RecoveryWidget::flashZipCopied()
{
    disconnect(this->dialog,SIGNAL(finished(int)),this,SLOT(flashZipCopied()));

    this->procesNandroid.start("\"" + sdk + "\"adb shell echo '--update_package=/sdcard/" + this->romFileName + "'>/cache/recovery/command");

    this->ui->textFlashZIP->append(tr("Writing commands...."));

    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(flashZipReboot()));
}

void RecoveryWidget::flashZipReboot()
{
    this->ui->textFlashZIP->append(tr("Rebooting...."));
    this->procesNandroid.start("\"" + sdk + "\"adb shell reboot recovery");
    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(flashZipReboot()));
}
void RecoveryWidget::mountSDcard()
{
    QProcess *process=new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    QString tmp;
    if (this->recoverySDmounted)
        process->start("\""+sdk+"\"adb shell ums_toggle off");
    else
        process->start("\""+sdk+"\"adb shell ums_toggle on");
    this->recoverySDmounted = !this->recoverySDmounted;
    process->waitForFinished(-1);
    tmp = process->readAll();
    if (tmp.contains("enabled"))
    {
        QMessageBox::information(this, tr("USB mounted"), tr("SD card is mounted as usb drive"), QMessageBox::Ok);
    }
    else if (tmp.contains("disabled"))
    {
        QMessageBox::information(this, tr("USB unmounted"), tr("SD card is unmounted"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tmp, QMessageBox::Ok);
    }
    process->terminate();
    delete process;
}

void RecoveryWidget::nandroidBackup()
{
    QString command;
    command = "adb shell " + this->nandroidScript + " --backup";
    if (this->ui->checkNandroidBackupPath->isChecked())
        command.append(" --path " + this->ui->editNandroidBackupPath->text());
    if (this->ui->checkNandroidBackupString->isChecked())
        command.append(" -s: " + this->ui->editNandroidBackupString->text());
    if (this->ui->checkNandroidBackupExt->isChecked())
        command.append(" -e");
    if (!this->ui->checkNandroidBackupBoot->isChecked())
        command.append(" --noboot");
    if (!this->ui->checkNandroidBackupCache->isChecked())
        command.append(" --nocache");
    if (!this->ui->checkNandroidBackupData->isChecked())
        command.append(" --nodata");
    if (!this->ui->checkNandroidBackupRecovery->isChecked())
        command.append(" --norecovery");
    if (!this->ui->checkNandroidBackupSystem->isChecked())
        command.append(" --nosystem");
    if (!this->ui->checkNandroidBackupMisc->isChecked())
        command.append(" --nomisc");
    command.append(" --defaultinput");
    this->ui->textNandroidBackup->clear();
    this->procesNandroid.setProcessChannelMode(QProcess::MergedChannels);
    this->procesNandroid.start("\"" + sdk + "\"" + command);
    connect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(nandroidBackupOutput()));
    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidBackupFinished()));
    this->ui->buttonNandroidBackupStart->setDisabled(true);
}

void RecoveryWidget::nandroidBackupEdit()
{
    if (this->ui->checkNandroidBackupPath->isChecked())
        this->ui->editNandroidBackupPath->setEnabled(true);
    else
        this->ui->editNandroidBackupPath->setDisabled(true);
    if (this->ui->checkNandroidBackupString->isChecked())
        this->ui->editNandroidBackupString->setEnabled(true);
    else
        this->ui->editNandroidBackupString->setDisabled(true);

}

void RecoveryWidget::nandroidBackupFinished()
{
    this->procesNandroid.terminate();
    this->ui->buttonNandroidBackupStart->setEnabled(true);
    disconnect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(nandroidBackupOutput()));
    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidBackupFinished()));
    QMessageBox::information(this, tr("Backup finished!"), tr("Nandroid backup is finished."), QMessageBox::Ok);
}

void RecoveryWidget::nandroidBackupOutput()
{
    QString output;
    output = this->procesNandroid.readAll();
    int i;
    for (i = 0; i < output.length(); i++)
    {
        if (output.at(i).unicode() == 13)
            output[i] = ' ';
        if (output.at(i).unicode() == 10)
            output[i] = '\n';
    }
    this->ui->textNandroidBackup->insertPlainText(output);
}

void RecoveryWidget::nandroidBackupPage()
{
    this->ui->checkNandroidBackupString->setChecked(false);
    this->ui->checkNandroidBackupPath->setChecked(false);
    this->ui->checkNandroidBackupCache->setChecked(false);
    this->ui->checkNandroidBackupData->setChecked(true);
    this->ui->checkNandroidBackupBoot->setChecked(true);
    this->ui->checkNandroidBackupRecovery->setChecked(false);
    this->ui->checkNandroidBackupSystem->setChecked(true);
    this->ui->checkNandroidBackupMisc->setChecked(false);
    this->nandroidBackupEdit();
    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageNandroidBackup);
}

void RecoveryWidget::nandroidRestore()
{
    QString command, tmp;
    command = "adb shell " + this->nandroidScript + "--restore";
    if (this->ui->checkNandroidRestoreExt->isChecked() && this->ui->checkNandroidRestoreExt->isEnabled())
        command.append(" -e");
    if (!this->ui->checkNandroidRestoreBoot->isChecked() && this->ui->checkNandroidRestoreBoot->isEnabled())
        command.append(" --noboot");
    if (!this->ui->checkNandroidRestoreCache->isChecked() && this->ui->checkNandroidRestoreCache->isEnabled())
        command.append(" --nocache");
    if (!this->ui->checkNandroidRestoreData->isChecked() && this->ui->checkNandroidRestoreData->isEnabled())
        command.append(" --nodata");
    if (!this->ui->checkNandroidRestoreRecovery->isChecked() && this->ui->checkNandroidRestoreRecovery->isEnabled())
        command.append(" --norecovery");
    if (!this->ui->checkNandroidRestoreSystem->isChecked() && this->ui->checkNandroidRestoreSystem->isEnabled())
        command.append(" --nosystem");
    if (!this->ui->checkNandroidRestoreMisc->isChecked() && this->ui->checkNandroidRestoreMisc->isEnabled())
        command.append(" --nomisc");
    tmp = this->ui->comboNandroidRestore->currentText();
    tmp = tmp.split("/", QString::SkipEmptyParts).last();
    command.append(" --defaultinput -s " + tmp);

    this->ui->textNandroidRestore->clear();
    this->procesNandroid.setProcessChannelMode(QProcess::MergedChannels);
    this->procesNandroid.start("\"" + sdk + "\"" + command);
    connect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(nandroidRestoreOutput()));
    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidRestoreFinished()));
    this->ui->buttonNandroidRestoreStart->setDisabled(true);
}

void RecoveryWidget::nandroidRestoreCombo()
{
    QString tmpStr;

    QProcess tmp;
    tmp.start("\""+sdk+"\"adb shell mount /sdcard");
    tmp.waitForFinished(-1);
    tmp.terminate();
    tmpStr = this->ui->comboNandroidRestore->currentText();

    FileList *fileList = Phone::getStaticFileList(tmpStr,this->sdk,true);

    if (fileList->name.contains("boot.img"))
    {
        this->ui->checkNandroidRestoreBoot->setEnabled(true);
        this->ui->checkNandroidRestoreBoot->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreBoot->setChecked(false);
        this->ui->checkNandroidRestoreBoot->setDisabled(true);
    }
    if (fileList->name.contains("cache.img"))
    {
        this->ui->checkNandroidRestoreCache->setEnabled(true);
        this->ui->checkNandroidRestoreCache->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreCache->setChecked(false);
        this->ui->checkNandroidRestoreCache->setDisabled(true);
    }
    if (fileList->name.contains("data.img"))
    {
        this->ui->checkNandroidRestoreData->setEnabled(true);
        this->ui->checkNandroidRestoreData->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreData->setChecked(false);
        this->ui->checkNandroidRestoreData->setDisabled(true);
    }
    if (fileList->name.contains("misc.img"))
    {
        this->ui->checkNandroidRestoreMisc->setEnabled(true);
        this->ui->checkNandroidRestoreMisc->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreMisc->setChecked(false);
        this->ui->checkNandroidRestoreMisc->setDisabled(true);
    }
    if (fileList->name.contains("recovery.img"))
    {
        this->ui->checkNandroidRestoreRecovery->setEnabled(true);
        this->ui->checkNandroidRestoreRecovery->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreRecovery->setChecked(false);
        this->ui->checkNandroidRestoreRecovery->setDisabled(true);
    }
    if (fileList->name.contains("system.img"))
    {
        this->ui->checkNandroidRestoreSystem->setEnabled(true);
        this->ui->checkNandroidRestoreSystem->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreSystem->setChecked(false);
        this->ui->checkNandroidRestoreSystem->setDisabled(true);
    }
    if (fileList->name.contains("ext.img"))
    {
        this->ui->checkNandroidRestoreExt->setEnabled(true);
        this->ui->checkNandroidRestoreExt->setChecked(true);
    }
    else
    {
        this->ui->checkNandroidRestoreExt->setChecked(false);
        this->ui->checkNandroidRestoreExt->setDisabled(true);
    }

    tmp.start("\""+sdk+"\"adb shell umount /sdcard");
    tmp.waitForFinished(-1);
    tmp.terminate();
}

void RecoveryWidget::nandroidRestoreFinished()
{
    this->procesNandroid.terminate();
    this->ui->buttonNandroidRestoreStart->setEnabled(true);
    disconnect(&this->procesNandroid, SIGNAL(readyRead()), this, SLOT(nandroidBackupOutput()));
    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidRestoreFinished()));
    QMessageBox::information(this, tr("Restore finished!"), tr("Nandroid restore is finished."), QMessageBox::Ok);
}

void RecoveryWidget::nandroidRestoreFound()
{
    int i, listSize;
    QString tmpStr;

    this->procesNandroid.terminate();
    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidRestoreFound()));
    tmpStr = this->procesNandroid.readAll();
    for (i = 0; i < tmpStr.length(); i++)
    {
        if (tmpStr.at(i).unicode() == 13)
            tmpStr[i] = ' ';
        if (tmpStr.at(i).unicode() == 10)
            tmpStr[i] = '\n';
    }
    QStringList listaBak = tmpStr.split("\n", QString::SkipEmptyParts);
    this->ui->comboNandroidRestore->clear();
    listSize = listaBak.length();
    for (i = 0; i < listSize; i++)
    {
        tmpStr = listaBak.takeFirst();
        tmpStr.remove(QRegExp("\\s+$"));
        if (tmpStr.startsWith("/"))
            this->ui->comboNandroidRestore->addItem(tmpStr);
    }

    if (tmpStr.contains("Error"))
    {
        QMessageBox::warning(this, "Error", tmpStr, QMessageBox::Ok);
        this->ui->textNandroidRestore->insertPlainText(tr("Finished"));
    }
    else
        this->ui->textNandroidRestore->insertPlainText(tr("Finished\nChoose one backup from combo above to restore it"));
    this->nandroidRestoreCombo();
}

void RecoveryWidget::nandroidRestoreOutput()
{
    QString output;
    output = this->procesNandroid.readAll();
    int i;
    for (i = 0; i < output.length(); i++)
    {
        if (output.at(i).unicode() == 13)
            output[i] = ' ';
        if (output.at(i).unicode() == 10)
            output[i] = '\n';
    }
    this->ui->textNandroidRestore->insertPlainText(output);
}

void RecoveryWidget::nandroidRestorePage()
{
    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageNandroidRestore);
    this->ui->textNandroidRestore->append(tr("Looking for backups..."));
    this->procesNandroid.setProcessChannelMode(QProcess::MergedChannels);
    this->procesNandroid.start("\""+sdk+"\"adb shell nandroid-mobile.sh --listbackup");
    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(nandroidRestoreFound()));
    //    tmp->waitForFinished(-1);

}

void RecoveryWidget::wipeBattery()
{
    QString output;
    QProcess *process=new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start("\""+sdk+"\"adb shell wipe battery");
    process->waitForFinished(-1);
    process->terminate();
    output = process->readAll();
    delete process;
    QMessageBox::information(this, tr("Information"), output, QMessageBox::Ok);
}

void RecoveryWidget::wipeData()
{
    if (QMessageBox::question(this, tr("Wipe data"), tr("Are you sure??"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;
    this->procesNandroid.setProcessChannelMode(QProcess::MergedChannels);
    this->procesNandroid.start("\"" + sdk + "\"adb shell echo \'--wipe_data\'>/cache/recovery/command");

    this->ui->textWipeData->append(tr("Writing commands...."));

    connect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(wipeDataFinished()));
    this->ui->stackedRecovery->setCurrentWidget(this->ui->pageWipeData);
}

void RecoveryWidget::wipeDataFinished()
{
    this->procesNandroid.terminate();
    this->procesNandroid.start("\"" + sdk + "\"adb shell reboot recovery");

    this->ui->textWipeData->append(tr("Rebooting...."));

    disconnect(&this->procesNandroid, SIGNAL(finished(int)), this, SLOT(wipeDataFinished()));
}
