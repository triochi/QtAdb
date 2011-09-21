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


#ifndef APPINFO_H
#define APPINFO_H

#include <QDialog>
#include <QIcon>
#include <QString>
#include <QNetworkReply>
#include <QPixmap>
#include "appdialog.h"
#include "../classes/models/apptablemodel.h"

namespace Ui {
    class appInfo;
}

class appInfo : public QDialog
{
    Q_OBJECT
public:
    App * app;
    appDialog *appsDialog;

public:
    appInfo(QWidget *parent, App *app);
    appInfo(App *app);
    ~appInfo();
private slots:
    void install();
private:
    Ui::appInfo *ui;
    bool reinstall;
    //qrcode
    void getQR();
    QNetworkReply * reply;
    QNetworkAccessManager * updateMan;
public:
    static QPixmap getQR(QString packageName);
    static QString getCyrketVer(QString packageName);
public slots:
    void gotWWW(QNetworkReply *);
    void openMarket();
    void openMarketPC();

};

#endif // APPINFO_H
