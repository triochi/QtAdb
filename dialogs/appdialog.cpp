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

appDialog::appDialog(QWidget *parent,QList<App> appList, int operation, int mode) :
    QDialog(parent),
    ui(new Ui::appDialog)
{
    QSettings settings;
    ui->setupUi(this);
    this->end=false;
    this->setWindowTitle(operation+" window");
    ui->label->setText(operation+".");
    this->setLayout(ui->horizontalLayout);
    this->timer=new QTimer(this);
    this->clock=new QTimer(this);
    this->appList=appList;
    this->sdk=settings.value("sdkPath").toString();
    this->licznik=0;
    this->mode = mode;
    this->proces = new QProcess(this);
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
        }this->restore();
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
    this->sdk=settings.value("sdkPath").toString();
    this->licznik=0;
    this->mode = mode;
    this->proces = new QProcess(this);
    this->operation = operation;
    proces->setProcessChannelMode(QProcess::MergedChannels);
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
        }this->restore();
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
    this->threadBackup->sdk = this->sdk;

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
        if (QMessageBox::question(this, tr("Cancel operation??"), tr("Are you sure You want to cancel operation??"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
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
    this->threadInstall->sdk = this->sdk;

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
    this->threadInstall->sdk = this->sdk;

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
    this->threadRestore->sdk = this->sdk;

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
    this->threadUninstall->sdk = this->sdk;

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
    QString output;
    QSettings settings;
    QByteArray ba;
    App app;
    proces->start("\"" + this->sdk + "\"" + "adb shell busybox mkdir /sdcard/QtADB/backup");
    proces->waitForFinished(-1);
    qDebug()<<"mkdir - "<<proces->readAll();
    while (this->appList.size() > 0)
    {
        app = this->appList.takeFirst();
//        name = this->appList.name.takeFirst();
//        package = this->appList.package.takeFirst();
//        file = this->appList.filename.takeFirst();
//        size = this->appList.size.takeFirst();
//        version = this->appList.version.takeFirst();

        emit this->nextApp(app);
        proces->start("\""+this->sdk+"\""+"adb shell echo -e \"app.name="+codec->toUnicode(app.appName.toUtf8())+"\" > /sdcard/QtADB/backup/"+app.packageName+".txt");
        proces->waitForFinished(-1);
        proces->start("\""+this->sdk+"\""+"adb shell echo -e \"app.size="+app.appSize+"\" >> /sdcard/QtADB/backup/"+app.packageName+".txt");
        proces->waitForFinished(-1);
        proces->start("\""+this->sdk+"\""+"adb shell echo -e \"app.version="+app.appVersion+"\" >> /sdcard/QtADB/backup/"+app.packageName+".txt");
        proces->waitForFinished(-1);
        ba = settings.value("apps/"+app.packageName+"/icon").toByteArray();
        QFile ikona(QDir::currentPath()+"/icons/"+app.packageName+".png");
        if (ikona.open(QIODevice::WriteOnly))
        {
            ikona.write(ba);
        }
        ikona.close();
        proces->start("\""+this->sdk+"\""+"adb push \""+QDir::currentPath()+"/icons/"+app.packageName+".png\" /sdcard/QtADB/backup/");
        proces->waitForFinished(-1);
        output = proces->readAll();
        qDebug()<<"Backup app - "<<output;
        ikona.open(QIODevice::ReadWrite);
        ikona.remove();
        if (this->withData)
        {
            proces->start("\""+this->sdk+"\""+"adb shell tar -zcf /sdcard/QtADB/backup/"+app.packageName+".DATA.tar.gz /data/data/"+app.packageName);
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - "<<output;
        }
        if (this->withApk)
        {
            proces->start("\""+this->sdk+"\""+"adb shell cp "+app.appFile+ " /sdcard/QtADB/backup/"+app.packageName+".apk");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Backup app - "<<output;
        }
        emit this->backedUp(tr("SUCCESS"), "");
    }
    emit this->backedUp("finished", "");
    return;
}

void ThreadRestore::run()
{
    QProcess *proces = new QProcess;
    QString output, userId;
    App app;
    while (this->appList.size() > 0)
    {
//        package = this->appList.package.takeFirst();
        app = this->appList.takeFirst();
        emit this->nextApp(app);
        if (this->withApk)
        {
            proces->start("\""+this->sdk+"\""+"adb shell pm install /sdcard/QtADB/backup/" +app.packageName + ".apk");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore pm - "<<output;
            if (output.contains("Failure [INSTALL_FAILED_INSUFFICIENT_STORAGE]"))
            {
                proces->start("\""+this->sdk+"\""+"adb shell cp /sdcard/QtADB/backup/" +app.packageName + ".apk /data/local/tmp/");
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore cp - "<<output;
                proces->start("\""+this->sdk+"\""+"adb shell pm install /data/local/tmp/" +app.packageName + ".apk");
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore pm - "<<output;
                proces->start("\""+this->sdk+"\""+"adb shell busybox rm -f /data/local/tmp/" +app.packageName + ".apk");
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
                    proces->start("\""+this->sdk+"\""+"adb shell busybox rm -rf /data/data/"+app.packageName);
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore rm - "<<output;
                    proces->start("\""+this->sdk+"\""+"adb shell busybox tar -xzf /sdcard/QtADB/backup/"+app.packageName+".DATA.tar.gz -C /");
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore tar - "<<output;
                    proces->start("\""+this->sdk+"\""+"adb shell \"cat /data/system/packages.xml|busybox grep '^<package.*"+app.packageName+"'\"");
                    proces->waitForFinished(-1);
                    output = proces->readAll();
                    qDebug()<<"Restore cat - "<<output;
                    int start = output.indexOf("serId=")+7;
                    if (start > 7)
                    {
                        userId = output.mid(start, output.indexOf("\"", start) - start);
                        proces->start("\""+this->sdk+"\""+"adb shell busybox chown -R "+userId+":"+userId+" /data/data/"+app.packageName);
                        proces->waitForFinished(-1);
                        output = proces->readAll();
                        qDebug()<<"Restore chown - "<<output;
                        proces->start("\""+this->sdk+"\""+"adb shell busybox chmod -R 775 /data/data/"+app.packageName);
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
            proces->start("\""+this->sdk+"\""+"adb shell busybox rm -rf /data/data/"+app.packageName);
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore rm - "<<output;
            proces->start("\""+this->sdk+"\""+"adb shell busybox tar -xzf /sdcard/QtADB/backup/"+app.packageName+".DATA.tar.gz -C /");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore tar - "<<output;
            proces->start("\""+this->sdk+"\""+"adb shell \"cat /data/system/packages.xml|busybox grep '^<package.*"+app.packageName+"'\"");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Restore cat - "<<output;
            int start = output.indexOf("serId=")+7;
            if (start > 7)
            {
                userId = output.mid(start, output.indexOf("\"", start) - start);
                proces->start("\""+this->sdk+"\""+"adb shell busybox chown -R "+userId+":"+userId+" /data/data/"+app.packageName);
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Restore chown - "<<output;
                proces->start("\""+this->sdk+"\""+"adb shell busybox chmod -R 775 /data/data/"+app.packageName);
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
    }
    emit this->restored("finished", "");
}

void ThreadInstall::run()
{
    QProcess *proces = new QProcess;
    QString output;

    proces->setReadChannelMode(QProcess::MergedChannels);
    App app;
    while (this->appList.length()>0)
    {
//        file = this->appList.package.takeFirst();
        app = this->appList.takeFirst();
        emit this->nextApp(app);
        QString cmd = "\""+this->sdk+"\""+"adb install " + (this->reinstall ? "-r " : "") + "\""+app.appFile+"\"";
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
            proces->start("\""+this->sdk+"\""+"adb remount");
            proces->waitForFinished(-1);
            output = proces->readAll();
            qDebug()<<"Remove system - "<<output;
            if (this->keepData)
            {
                proces->start("\""+this->sdk+"\""+"adb shell busybox rm -rf /data/data/"+app.packageName);
                proces->waitForFinished(-1);
                output = proces->readAll();
                qDebug()<<"Remove system - "<<output;
            }
            proces->start("\""+this->sdk+"\""+"adb shell busybox rm -rf "+app.appFile);
        }
        else
        {
            if (this->keepData)
                proces->start("\""+this->sdk+"\""+"adb uninstall -k "+app.packageName);
            else
                proces->start("\""+this->sdk+"\""+"adb uninstall "+app.packageName);
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
    }
    delete proces;
    emit this->uninstalled("finished", "");
}
