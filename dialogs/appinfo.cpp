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


#include "appinfo.h"
#include "ui_appinfo.h"

appInfo::appInfo(QWidget *parent, App *app) :
    QDialog(parent),
    ui(new Ui::appInfo)
{

    ui->setupUi(this);
    this->setLayout(this->ui->gridLayout);
    this->resize(350, 280);
    this->setFixedHeight(280);
    //this->setFixedSize(this->width(),this->height());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->app = app;
    this->app->packageName.remove(QRegExp("\\s+$"));

    ui->labelAppsIcon->setPixmap(this->app->appIcon.pixmap(100,100));
    ui->editAppsAppName->setText(this->app->appName);
    ui->editAppsVersion->setText(this->app->appVersion);
 //   ui->editAppsSize->setText(this->app->appSize);
    QString strTmp = " KB";
    long double sizeTmp = 0.0;
    sizeTmp = this->app->appSize.toLongLong();
    if (sizeTmp <= 1024)
    {
       strTmp.prepend(QString::number(sizeTmp, 'f', 0));
       ui->editAppsSize->setText(strTmp);
       strTmp = "";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " KB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " MB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " GB";
    }
    if (!strTmp.isEmpty())
    {
        strTmp.prepend(QString::number(sizeTmp, 'f', 3));
        ui->editAppsSize->setText(strTmp);
    }



    ui->editAppsFileName->setText(this->app->appFile);
    ui->editAppsPackageName->setText(this->app->packageName);

    this->updateMan = new QNetworkAccessManager(this);
    this->reply = NULL;
    this->appsDialog = NULL;
    this->reinstall = false;

    QString sdk;
    QSettings settings;
    sdk = settings.value("sdkPath").toString();
    QProcess proc;
    proc.start("\"" + sdk + "\"adb shell ls /data/app/"
               + this->app->packageName + "*");
    proc.waitForFinished(-1);
    QString output = proc.readAll();
    if (!output.contains("No such file or directory"))
    {
        this->ui->pushButton->setText(tr("Reinstall"));
        this->reinstall = true;
    }

    connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(install()));
    connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(openMarket()));
    connect(this->updateMan, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotWWW(QNetworkReply*)));
    connect(this->ui->pushButton_3, SIGNAL(clicked()), this, SLOT(openMarketPC()));
    this->getQR();

//    ui->pageApps->setLayout(ui->layoutApps);
}

appInfo::appInfo(App *app) :
    ui(new Ui::appInfo)
{

    ui->setupUi(this);
    this->setLayout(this->ui->gridLayout);
    this->resize(350, 280);
    this->setFixedHeight(280);
    this->app = app;

    ui->labelAppsIcon->setPixmap(this->app->appIcon.pixmap(100,100));
    ui->editAppsAppName->setText(this->app->appName);
    ui->editAppsVersion->setText(this->app->appVersion);
   // ui->editAppsSize->setText(this->app->appSize);
    QString strTmp = " KB";
    long double sizeTmp = 0.0;
    sizeTmp = this->app->appSize.toLongLong();
    if (sizeTmp <= 1024)
    {
       strTmp.prepend(QString::number(sizeTmp, 'f', 0));
       ui->editAppsSize->setText(strTmp);
       strTmp = "";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " KB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " MB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " GB";
    }
    if (!strTmp.isEmpty())
    {
        strTmp.prepend(QString::number(sizeTmp, 'f', 3));
        ui->editAppsSize->setText(strTmp);
    }


    ui->editAppsFileName->setText(this->app->appFile);
    ui->editAppsPackageName->setText(this->app->packageName);

    this->updateMan = new QNetworkAccessManager(this);
    this->reply = NULL;
    this->appsDialog = NULL;
    this->reinstall = false;

    QString sdk;
    QSettings settings;
    sdk = settings.value("sdkPath").toString();
    QProcess proc;
    proc.start("\"" + sdk + "\"adb shell ls /data/app/"
               + this->app->packageName + "*");
    proc.waitForFinished(-1);
    QString output = proc.readAll();
    if (!output.contains("No such file or directory"))
    {
        this->ui->pushButton->setText(tr("Reinstall"));
        this->reinstall = true;
    }


    connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(install()));
    connect(this->ui->pushButton_2, SIGNAL(clicked()), this, SLOT(openMarket()));
    connect(this->ui->pushButton_3, SIGNAL(clicked()), this, SLOT(openMarketPC()));
    connect(this->updateMan, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotWWW(QNetworkReply*)));
    this->getQR();

//    ui->pageApps->setLayout(ui->layoutApps);
}

appInfo::~appInfo()
{
    if (this->appsDialog != NULL)
        delete this->appsDialog;
    delete this->app;
    delete this->reply;

    delete ui;
}

void appInfo::install()
{
    QList<App> appList;
    appList.append(*this->app);
//    selected.package.append(this->app->appFile);

    if (QMessageBox::question(this,this->reinstall ? tr("Reinstall:") : tr("Install:"),tr("Are you sure???"),QMessageBox::Ok | QMessageBox::No) == QMessageBox::No)
        return;

    if (this->appsDialog != NULL)
        delete this->appsDialog;
    this->appsDialog=new appDialog(this,appList, this->reinstall ? appDialog::Reinstall : appDialog::Install, appDialog::None);
    this->appsDialog->show();
}


void appInfo::gotWWW(QNetworkReply * pReply)
{
    QByteArray data = pReply->readAll();
    QPixmap pix;
    pix.loadFromData(data);
    this->ui->labelQRcode->setPixmap(pix);
}

void appInfo::getQR()
{
    this->reply = this->updateMan->get(QNetworkRequest(QUrl("http://qrcode.kaywa.com/img.php?s=2&d=market://details?id="+this->app->packageName)));
}

void appInfo::openMarket()
{
    QString sdk;
    QSettings settings;
    sdk = settings.value("sdkPath").toString();
    QProcess proc;
    proc.start("\"" + sdk + "\"adb shell am start -a android.intent.action.VIEW -d market://details?id="
               + this->ui->editAppsPackageName->text() + " -n com.android.vending/.AssetBrowserActivity");
    proc.waitForFinished(-1);
    QString out = proc.readAll();
    if (out.contains("Error"))
    {
        proc.start("\"" + sdk + "\"adb shell am start -a android.intent.action.VIEW -d market://details?id="
                   + this->ui->editAppsPackageName->text() + " -n com.android.vending/com.google.android.finsky.activities.PlayLauncherActivity");
        proc.waitForFinished(-1);
    }
    qDebug()<<"adb shell am start -a android.intent.action.VIEW -d market://details?id="<<this->app->packageName<<" -n com.android.vending/.AssetBrowserActivity";
}

QPixmap appInfo::getQR(QString packageName)
{
    QNetworkAccessManager *nac = new QNetworkAccessManager;
    QNetworkReply *rep = nac->get(QNetworkRequest(QUrl("https://qrcode.kaywa.com/img.php?s=2&d=market://details?id=" + packageName)));
    while (true)
    {
        qApp->processEvents();
        if (rep->isFinished())
            break;
    }
    QByteArray ba;
    ba = rep->readAll();
    QPixmap pix;
    pix.loadFromData(ba);

    delete rep;
    delete nac;
    return pix;
}

QString appInfo::getCyrketVer(QString packageName)

{
    QNetworkAccessManager *nac = new QNetworkAccessManager;
    QNetworkReply *rep = nac->get(QNetworkRequest(QUrl("https://play.google.com/store/apps/details?id=" + packageName)));
   // qDebug()<<"packageName = "<<packageName;
   // qDebug()<<"QUrl http://market.android.com/details?id="<<packageName;

        QEventLoop loop;
        connect(rep, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

//   old: Current Version:</div>2.0<div
//   new: Current Version:</dt><dd itemprop="softwareVersion">1.0</dd><dt
    QByteArray ba = rep->readAll();
   // qDebug()<<"ba = "<<ba;
    QString str = "";
    int start = ba.indexOf("Current Version:</dt>");
//    start = ba.indexOf("<div>", start);
//    start+=5;
    if (start != -1)
    {
        start+=52;
        int end = ba.indexOf("</dd><dt", start);
        str = ba.mid(start, end-start);
       // qDebug()<<"Apps Version (found) str = "<<str;
    }
    delete rep;
    delete nac;
  //  qDebug()<<"Apps Version  (not found) str (version) = "<<str;
    if (str.isEmpty())
    {
       return "app not found on Android Market";
    }
    else if (str == "Varies with device")
    {
        return str + ", Open in Market";
    }
    else
    return str;
}

void appInfo::openMarketPC()
{
    QDesktopServices::openUrl(QUrl("http://market.android.com/details?id=" + this->ui->editAppsPackageName->text()));
}
