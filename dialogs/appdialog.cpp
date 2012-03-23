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


#include "appdialog.h"
#include "ui_appdialog.h"

extern QString sdk;
extern QString adb;
extern QString aapt;
extern QProcess *adbProces;
extern QString busybox;
extern QString fastboot;
appDialog::appDialog(QWidget *parent,QList<App> appList, int operation, int mode) :
    QDialog(parent),
    ui(new Ui::appDialog)
{
    QSettings settings;
   // this->setFixedSize(this->width(),this->height());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    this->end=false;
    this->setWindowTitle(operation+" window");
    ui->label->setText(operation+".");
    this->setLayout(ui->horizontalLayout);
    this->timer=new QTimer(this);
    this->clock=new QTimer(this);
    this->appList=appList;
    this->licznik=0;
    this->mode = mode;
    this->operation = operation;
    proces->setProcessChannelMode(QProcess::MergedChannels);
    this->ui->progressBar->setMaximum(appList.size());
    this->ui->tableWidget->horizontalHeader()->setVisible(true);

    if (this->operation == appDialog::Install)
    {
        this->setWindowTitle(tr("Install"));
        ui->label->setText(tr("Install"));
        this->install();
    }
    else if (this->operation == appDialog::Uninstall)
    {
        this->setWindowTitle(tr("Uninstall"));
        ui->label->setText(tr("Uninstall"));
        this->uninstall();
    }
    else if (this->operation == appDialog::Reinstall)
    {
        this->setWindowTitle(tr("Reinstall"));
        ui->label->setText(tr("Reinstall"));
        this->reinstall();
    }
    else if (this->operation == appDialog::Backup)
    {
        if (this->mode == appDialog::Application)
        {
            this->setWindowTitle(tr("Backup apps"));
            ui->label->setText(tr("Backup apps"));
        }
        else if (this->mode == appDialog::Data)
        {
            this->setWindowTitle(tr("Backup data"));
            ui->label->setText(tr("Backup data"));
        }
        else if (this->mode == appDialog::AppAndData)
        {
            this->setWindowTitle(tr("Backup apps and data"));
            ui->label->setText(tr("Backup apps and data"));
        }
        this->backup();
    }
    else if (this->operation == appDialog::Restore)
    {
        if (this->mode == appDialog::Application)
        {
            this->setWindowTitle(tr("Restore apps"));
            ui->label->setText(tr("Restore apps"));
        }
        else if (this->mode == appDialog::Data)
        {
            this->setWindowTitle(tr("Restore data"));
            ui->label->setText(tr("Restore data"));
        }
        else if (this->mode == appDialog::AppAndData)
        {
            this->setWindowTitle(tr("Restore apps and data"));
            ui->label->setText(tr("Restore apps and data"));
        }
        this->restore();
    }
    this->timer->start(50);
    this->clock->start(100);

    connect(this->clock,SIGNAL(timeout()),this,SLOT(clockTimeout()));
}

appDialog::appDialog(QList<App> appList, int operation, int mode) :
    ui(new Ui::appDialog)
{
    QSettings settings;
    ui->setupUi(this);
    this->end=false;
//    this->setWindowTitle(operation+" window");
//    ui->label->setText(operation+".");
    this->setLayout(ui->horizontalLayout);
    this->timer=new QTimer(this);
    this->clock=new QTimer(this);
    this->appList=appList;
    this->licznik=0;
    this->mode = mode;
    this->operation = operation;
    this->ui->progressBar->setMaximum(appList.size());
    this->ui->tableWidget->horizontalHeader()->setVisible(true);

    if (this->operation == appDialog::Install)
    {
        this->setWindowTitle(tr("Install", "apd dialog title"));
        ui->label->setText(tr("Install", "label text"));
        this->install();
    }
    else if (this->operation == appDialog::Uninstall)
    {
        this->setWindowTitle(tr("Uninstall", "apd dialog title"));
        ui->label->setText(tr("Uninstall", "label text"));
        this->uninstall();
    }
    else if (this->operation == appDialog::Reinstall)
    {
        this->setWindowTitle(tr("Reinstall"));
        ui->label->setText(tr("Reinstall"));
        this->reinstall();
    }
    else if (this->operation == appDialog::Backup)
    {
        if (this->mode == appDialog::Application)
        {
            this->setWindowTitle(tr("Backup apps", "apd dialog title"));
            ui->label->setText(tr("Backup apps", "label text"));
        }
        else if (this->mode == appDialog::Data)
        {
            this->setWindowTitle(tr("Backup data", "apd dialog title"));
            ui->label->setText(tr("Backup data", "label text"));
        }
        else if (this->mode == appDialog::AppAndData)
        {
            this->setWindowTitle(tr("Backup apps and data", "apd dialog title"));
            ui->label->setText(tr("Backup apps and data", "label text"));
        }
        this->backup();
    }
    else if (this->operation == appDialog::Restore)
    {
        if (this->mode == appDialog::Application)
        {
            this->setWindowTitle(tr("Restore apps", "apd dialog title"));
            ui->label->setText(tr("Restore apps", "label text"));
        }
        else if (this->mode == appDialog::Data)
        {
            this->setWindowTitle(tr("Restore data", "apd dialog title"));
            ui->label->setText(tr("Restore data", "label text"));
        }
        else if (this->mode == appDialog::AppAndData)
        {
            this->setWindowTitle(tr("Restore apps and data"));
            ui->label->setText(tr("Restore apps and data", "label text"));
        }
        this->restore();
    }
    this->timer->start(50);
    this->clock->start(100);

    connect(this->clock,SIGNAL(timeout()),this,SLOT(clockTimeout()));
}

appDialog::~appDialog()
{
    delete ui;
}

void appDialog::backup()
{
    this->timer->stop();

    this->threadBackup = new ThreadBackup;

    this->threadBackup->appList = this->appList;
    if (this->mode == appDialog::Application)
    {
        this->threadBackup->withData = false;
        this->threadBackup->withApk = true;
    }
    else if (this->mode == appDialog::Data)
    {
        this->threadBackup->withData = true;
        this->threadBackup->withApk = false;
    }
    else if (this->mode == appDialog::AppAndData)
    {
        this->threadBackup->withData = true;
        this->threadBackup->withApk = true;
    }
    this->threadBackup->start();
    qRegisterMetaType<App>("App");
    connect(this->threadBackup,SIGNAL(backedUp(QString, QString)),this,SLOT(finished(QString, QString)));
    connect(this->threadBackup, SIGNAL(nextApp(App)), this, SLOT(nextApp(App)));
}

void appDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void appDialog::closeEvent(QCloseEvent *event)
{
    if (this->end)
    {
        emit this->closed();
        event->accept();
    }
    else
    {
        if (QMessageBox::question(this, tr("Cancel operation?"), tr("Are you sure you want to cancel operation??"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            event->ignore();
        }
        else
        {
            this->clock->stop();
            if (this->operation == appDialog::Install)
            {
                threadInstall->terminate();
            }
            else if (this->operation == appDialog::Uninstall)
            {
                threadUninstall->terminate();
            }
            else if (this->operation == appDialog::Backup)
            {
                threadBackup->terminate();
            }
            else if (this->operation == appDialog::Restore)
            {
                threadRestore->terminate();
            }

            emit this->closed();
            event->accept();
        }
    }
}

void appDialog::clockTimeout()
{
    QString text=ui->label->text();
    if (text.contains("..."))
        ui->label->setText(text.left(text.length()-3));
    else
        ui->label->setText(text+".");
}

void appDialog::install()
{
    this->timer->stop();
    this->threadInstall = new ThreadInstall;

    this->threadInstall->reinstall=false;
    this->threadInstall->appList = this->appList;
    this->threadInstall->start();
    qRegisterMetaType<App>("App");
    connect(this->threadInstall,SIGNAL(installed(QString, QString)),this,SLOT(finished(QString, QString)));
    connect(this->threadInstall, SIGNAL(nextApp(App)), this, SLOT(nextApp(App)));
}

void appDialog::reinstall()
{
    this->timer->stop();
    this->threadInstall = new ThreadInstall;

    this->threadInstall->reinstall=true;
    this->threadInstall->appList = this->appList;
    this->threadInstall->start();

    qRegisterMetaType<App>("App");
    connect(this->threadInstall,SIGNAL(installed(QString, QString)),this,SLOT(finished(QString, QString)));
    connect(this->threadInstall, SIGNAL(nextApp(App)), this, SLOT(nextApp(App)));
}

void appDialog::restore()
{
    this->timer->stop();

    this->threadRestore = new ThreadRestore;

    this->threadRestore->withData = false;
    this->threadRestore->withApk = false;
    this->threadRestore->appList = this->appList;
    if (this->mode == appDialog::Application)
    {
        this->threadRestore->withData = false;
        this->threadRestore->withApk = true;
    }
    else if (this->mode == appDialog::Data)
    {
        this->threadRestore->withData = true;
        this->threadRestore->withApk = false;
    }
    else if (this->mode == appDialog::AppAndData)
    {
        this->threadRestore->withData = true;
        this->threadRestore->withApk = true;
    }
    this->threadRestore->start();
    qRegisterMetaType<App>("App");
    connect(this->threadRestore,SIGNAL(restored(QString, QString)),this,SLOT(finished(QString, QString)));
    connect(this->threadRestore, SIGNAL(nextApp(App)), this, SLOT(nextApp(App)));
}

void appDialog::finished(QString status, QString reason)
{
    if (status != "finished")
    {
        QTableWidgetItem *item=new QTableWidgetItem;
        item->setText(status);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,item);
        item=new QTableWidgetItem;
        item->setText(reason);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,item);
        this->ui->tableWidget->resizeColumnsToContents();
    }
    else
    {
        this->clock->stop();
        if (this->operation == appDialog::Install)
        {
            QMessageBox::information(this,"",tr("Install finished"),QMessageBox::Ok);
        }
        else if (this->operation == appDialog::Uninstall)
        {
            QMessageBox::information(this,"",tr("Uninstall finished"),QMessageBox::Ok);
        }
        else if (this->operation == appDialog::Reinstall)
        {
            QMessageBox::information(this,"",tr("Reinstall finished"),QMessageBox::Ok);
        }
        else if (this->operation == appDialog::Backup)
        {
            QMessageBox::information(this,"",tr("Backup finished"),QMessageBox::Ok);
        }
        else if (this->operation == appDialog::Restore)
        {
            QMessageBox::information(this,"",tr("Restore finished"),QMessageBox::Ok);
        }

        this->end=true;
        this->close();
    }
}

void appDialog::uninstall()
{
    this->timer->stop();

    this->threadUninstall = new ThreadUninstall;

    this->threadUninstall->appList = this->appList;
    if (this->appList.first().appFile.contains(QRegExp("/system/app/.+")))
        this->threadUninstall->system = true;
    else
        this->threadUninstall->system = false;
    if (this->mode == appDialog::AppAndData)
        this->threadUninstall->keepData = false;
    else
        this->threadUninstall->keepData = true;
    this->threadUninstall->start();
    qRegisterMetaType<App>("App");
    connect(this->threadUninstall,SIGNAL(uninstalled(QString, QString)),this,SLOT(finished(QString, QString)));
    connect(this->threadUninstall, SIGNAL(nextApp(App)), this, SLOT(nextApp(App)));
}

void appDialog::nextApp(App app)
{
    QTableWidgetItem *item = new QTableWidgetItem;
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    item->setText(app.appName);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,item);
    item = new QTableWidgetItem;
    item->setIcon(app.appIcon);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,item);

    ui->tableWidget->setCurrentItem(item);
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resizeColumnsToContents();
    emit this->progressValue(this->ui->tableWidget->rowCount(), this->ui->progressBar->maximum());
    ui->progressBar->setValue(ui->tableWidget->rowCount());
}

void ThreadBackup::run()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QProcess *proces = new QProcess;
    QString output, iconfile;
    QSettings settings;
    QString backuponpc;
    bool onsdcard;
    QString appsBackupFolder = settings.value("appsBackupFolder").toString();
    QByteArray ba;
    App app;
    if (!appsBackupFolder.contains("/sdcard/"))
    {
        backuponpc = appsBackupFolder.append("/");
        appsBackupFolder = "/sdcard/tmpAppsBackup/";
        onsdcard = false;
        qDebug()<<"1----------Backup app on PC - "<<backuponpc<<"-----"<<appsBackupFolder;
    }
    else
       onsdcard = true;
    proces->start("\"" + adb + "\" shell " + busybox +  " mkdir \"" + codec->toUnicode(appsBackupFolder.toUtf8()) +"\"");
    proces->waitForFinished(-1);
    qDebug()<<"mkdir - "<<proces->readAll();
    while (this->appList.size() > 0)
    {
        app = this->appList.takeFirst();
        proces->start("\"" + adb + "\" shell " + busybox + " mkdir \"" + codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(app.appName.append("/").toUtf8())+"\"");
        proces->waitForFinished(-1);
        emit this->nextApp(app);
        proces->start("\""+adb + "\" shell " + busybox + " echo -e \"app.name="+codec->toUnicode(app.appName.left(app.appName.size()-1).toUtf8())+"\" > \""+ codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".txt\"");
        proces->waitForFinished(-1);
        proces->start("\""+adb + "\" shell " + busybox + " echo -e \"app.size="+app.appSize+"\" >> \""+ codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".txt\"");
        proces->waitForFinished(-1);
        proces->start("\""+adb + "\" shell " + busybox + " echo -e \"app.version="+app.appVersion+"\" >> \""+ codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".txt\"");
        proces->waitForFinished(-1);
        ba = settings.value("apps/"+app.packageName+"/icon").toByteArray();
        if (onsdcard == false)
        {
            QDir dir;
            dir.mkdir(codec->toUnicode(backuponpc.toUtf8())+codec->toUnicode(app.appName.toUtf8()));
            iconfile = codec->toUnicode(backuponpc.toUtf8())+codec->toUnicode(app.appName.append("/").toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".png";
        }
        else
            iconfile = QDir::currentPath()+"/tmp/"+codec->toUnicode(app.packageName.toUtf8())+".png";
        QFile ikona(iconfile);
        if (ikona.open(QIODevice::WriteOnly))
        {
            ikona.write(ba);
        }
        ikona.close();
        if (onsdcard == true)
        {
        proces->start("\""+adb + "\" shell " + busybox + " push \""+QDir::currentPath()+"/tmp/"+codec->toUnicode(app.packageName.toUtf8())+".png\" \""+ codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".png\"");
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<"Backup app - icon "<<output;
        ikona.remove();
        }
        if (this->withData)
        {
            proces->start("\""+adb + "\" shell " + busybox + " tar -zchf \"" + codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".DATA.tar.gz\" \"/data/data/"+codec->toUnicode(app.packageName.toUtf8())+"\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - "<<output;
        }
        if (this->withApk)
        {
            if (onsdcard == false)
                proces->start("\""+ adb + "pull \""+codec->toUnicode(app.appFile.toUtf8())+ "\" \"" + codec->toUnicode(backuponpc.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".apk\"");
            else
                proces->start("\""+adb + "\" shell " + busybox + " cp \""+codec->toUnicode(app.appFile.toUtf8())+ "\" \"" + codec->toUnicode(appsBackupFolder.toUtf8())+codec->toUnicode(app.appName.toUtf8())+codec->toUnicode(app.packageName.toUtf8())+".apk\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - "<<output;
        }
        if (onsdcard == false)
        {
            proces->start("\""+this->sdk+"\""+"adb pull \""+codec->toUnicode(appsBackupFolder.toUtf8())+"\" \""+ codec->toUnicode(backuponpc.toUtf8())+"\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - adb pull "<<output;
            proces->start("\""+this->sdk+"\""+"adb shell rm -r \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(app.appName.toUtf8())+"\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - rm -r "<<output;
            qDebug()<<"3----------Backup app on PC - "<<backuponpc<<"-----"<<appsBackupFolder;
        }
        emit this->backedUp(tr("SUCCESS"), "");
    }
        if (onsdcard == false)
        {
        proces->start("\""+this->sdk+"\""+"adb shell rm -r \""+codec->toUnicode(appsBackupFolder.toUtf8())+"\"");
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<"Backup app - rm -r "<<output;
        qDebug()<<"3----------Backup app on PC - "<<backuponpc<<"-----"<<appsBackupFolder;
        }
    emit this->backedUp("finished", "");
    return;
}

void ThreadRestore::run()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QProcess *proces = new QProcess;
    QString output, userId;
    App app;
    QSettings settings;
    QString backuponpc, namedir;
    bool onsdcard;
    QString appsBackupFolder = settings.value("appsBackupFolder").toString();
    if (!appsBackupFolder.contains("/sdcard/"))
    {
        backuponpc = appsBackupFolder.append("/");
        appsBackupFolder = "/sdcard/tmpAppsBackup/";
        proces->start("\"" + this->sdk + "\"" + "adb shell mkdir \"" + codec->toUnicode(appsBackupFolder.toUtf8()) +"\"");
        proces->waitForFinished(-1);
        qDebug()<<"mkdir /sdcard/tmpAppsBackup/ - "<<proces->readAll();
        onsdcard = false;
        qDebug()<<"1----------Backup app on PC - "<<backuponpc<<"-----"<<appsBackupFolder;
    }
    else
       onsdcard = true;
    while (this->appList.size() > 0)
    {
//        package = this->appList.package.takeFirst();
        app = this->appList.takeFirst();
        emit this->nextApp(app);
        namedir = app.appName.append("/");
        if (onsdcard == false)
        {
            proces->start("\"" +adb + "\" shell " + busybox + " mkdir \"" + codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8())+"\"");
            proces->waitForFinished(-1);
            qDebug()<<"mkdir /sdcard/tmpAppsBackup/appName/ - "<<proces->readAll();
        }
        if (this->withApk)
        {
            if (onsdcard == false)
            {
                proces->start("\""+adb + "\" push \"" + codec->toUnicode(backuponpc.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8()) + ".apk\" \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) + "\"");
                proces->waitForFinished(-1);
            }
            proces->start("\""+adb + "\" shell " + busybox + " pm install \"" + codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8()) + ".apk\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore pm - "<<output;
            if (output.contains("Failure [INSTALL_FAILED_INSUFFICIENT_STORAGE]"))
            {
                proces->start("\""+adb + "\" shell " + busybox + " cp \"" + codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8()) + ".apk\" /data/local/tmp/");
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore cp - "<<output;
                proces->start("\""+adb + "\" shell " + busybox + " pm install /data/local/tmp/" +codec->toUnicode(app.packageName.toUtf8()) + ".apk");
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore pm - "<<output;
                proces->start("\""+adb + "\" shell " + busybox + " rm -f /data/local/tmp/" +codec->toUnicode(app.packageName.toUtf8()) + ".apk");
                proces->waitForFinished(-1);
            }
            if (output.contains("Failure"))
            {
                QString reason = output.mid(output.indexOf("[") + 1, output.indexOf("]") - output.indexOf("[") - 1);

                emit this->restored(tr("FAILURE"), reason);
            }
            else if (output.contains("Success"), "")
            {
                if (this->withData)
                {
                    proces->start("\""+adb + "\" shell " + busybox + " rm -rf /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore rm - "<<output;
                    if (onsdcard == false)
                    {
                        proces->start("\""+adb + "\" + " push \"" + codec->toUnicode(backuponpc.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8()) + ".DATA.tar.gz\" \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) + "\"");
                        proces->waitForFinished(-1);
                    }
                    proces->start("\""+adb + "\" shell " + busybox + " tar -xzf \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8())+".DATA.tar.gz\" -C /");
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore tar - "<<output;
                    proces->start("\""+adb + "\" shell " + busybox + " \"cat /data/system/packages.xml | grep '^<package.*"+codec->toUnicode(app.packageName.toUtf8())+"'\"");
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore cat - "<<output;
                    int start = output.indexOf("serId=")+7;
                    if (start > 7)
                    {
                        userId = output.mid(start, output.indexOf("\"", start) - start);
                        proces->start("\""+adb + "\" shell " + busybox + " chown -R "+userId+":"+userId+" /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                        proces->waitForFinished(-1);
                        output = proces->readAll();
                        qDebug()<<"Restore chown - "<<output;
                        proces->start("\""+adb + "\" shell " + busybox + " chmod -R 775 /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                        proces->waitForFinished(-1);
                        output = proces->readAll();
                        qDebug()<<"Restore chmod - "<<output;
                        emit this->restored(tr("SUCCESS"), "");
                    }
                    else
                    {
                        emit this->restored(tr("FAILURE"), "");
                    }
                }
                else
                    emit this->restored(tr("SUCCESS"), "");
            }
        }
        else if (this->withData)
        {
            proces->start("\""+adb + "\" shell " + busybox + " rm -rf /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore rm - "<<output;
            if (onsdcard == false)
            {
                proces->start("\""+adb + "\" push \"" + codec->toUnicode(backuponpc.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8()) + ".DATA.tar.gz\" \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) + "\"");
                proces->waitForFinished(-1);
            }
            proces->start("\""+adb+"\""+" shell tar -xzf \""+codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8()) +codec->toUnicode(app.packageName.toUtf8())+".DATA.tar.gz\" -C /");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore tar - "<<output;
            proces->start("\""+adb + "\" shell " + busybox + " \"cat /data/system/packages.xml | grep '^<package.*"+codec->toUnicode(app.packageName.toUtf8())+"'\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore cat - "<<output;
            int start = output.indexOf("serId=")+7;
            if (start > 7)
            {
                userId = output.mid(start, output.indexOf("\"", start) - start);
                proces->start("\""+adb + "\" shell " + busybox + " chown -R "+userId+":"+userId+" /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore chown - "<<output;
                proces->start("\""+adb + "\" shell " + busybox + " chmod -R 775 /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore chmod - "<<output;
                emit this->restored(tr("SUCCESS"), "");
            }
            else
            {
                emit this->restored(tr("FAILURE"), "");
            }
        }
        if (onsdcard == false)
        {
            proces->start("\""+adb + "\" shell " + busybox + " rm -r \"" + codec->toUnicode(appsBackupFolder.toUtf8()) + codec->toUnicode(namedir.toUtf8())+"\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - rm -r "<<output;
        }
    }
    if (onsdcard == false)
    {
        proces->start("\""+adb + "\" shell " + busybox + " shell rm -r \""+codec->toUnicode(appsBackupFolder.toUtf8())+"\"");
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<"Backup app - rm -r "<<output;
    }
    emit this->restored("finished", "");
}

void ThreadInstall::run()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QProcess *proces = new QProcess;
    QString output;

    proces->setReadChannelMode(QProcess::MergedChannels);
    App app;
    while (this->appList.length()>0)
    {
//        file = this->appList.package.takeFirst();
        app = this->appList.takeFirst();
        emit this->nextApp(app);
        QString cmd = "\""+ adb + "\"" + QString(" install ") + (this->reinstall ? "-r " : "") + "\""+app.appFile+"\"";
        proces->start(cmd);
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<(this->reinstall ? "AppReInstall - " : "AppInstall - ")<<output;
        if (output.contains("Failure"))
        {
            QString reason = output.mid(output.indexOf("[") + 1, output.indexOf("]") - output.indexOf("[") - 1);
            emit this->installed(tr("FAILURE"), reason);
        }
        else if (output.contains("Success"))
        {
            emit this->installed(tr("SUCCESS"), "");
        }
    }
    emit this->installed("finished", "");
}

void ThreadUninstall::run()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QString output;
    QProcess *proces = new QProcess;

    App app;
    while (this->appList.size() > 0)
    {
//        name = this->appList.name.takeFirst();
        app = this->appList.takeFirst();
//        if (this->appList.filename.count() > 0)
//            fileName = this->appList.filename.takeFirst();
//        package = this->appList.package.takeFirst();

        emit this->nextApp(app);

        if (this->system)
        {
            proces->start("\""+adb+"\""+" shell mount -o remount,rw /system");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Remove system - "<<output;
            if (this->keepData)
            {
                proces->start("\""+adb + "\" shell " + busybox + "adb shell rm -rf /data/data/"+codec->toUnicode(app.packageName.toUtf8()));
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Remove system - "<<output;
            }
            proces->start("\""+adb + "\" shell " + busybox + " rm -rf "+app.appFile);
        }
        else
        {
            if (this->keepData)
                proces->start("\""+adb+"\" uninstall -k "+codec->toUnicode(app.packageName.toUtf8()));
            else
                proces->start("\""+adb+"\" uninstall "+codec->toUnicode(app.packageName.toUtf8()));
        }
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<"Remove app - "<<output;
        if (output.contains("Failure"))
        {
            QString reason = output.mid(output.indexOf("[") + 1, output.indexOf("]") - output.indexOf("[") - 1);

            emit this->uninstalled(tr("FAILURE"), reason);
        }
        else if (output.contains("Success"))
        {
            emit this->uninstalled(tr("SUCCESS"), "");
        }
        else
        {
            emit this->uninstalled(tr("SUCCESS"), "");
        }
        if (this->system)
        {
            proces->start("\""+adb + "\" shell " + busybox + " mount -o remount,ro,noatime /system");
            proces->waitForFinished(-1);
        }
    }
    delete proces;
    emit this->uninstalled("finished", "");
}
