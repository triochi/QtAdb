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


#include "registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent, QStringList accountList) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    this->setLayout(ui->gridLayout);
    this->accountList = accountList;
    this->registered = false;
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

bool isKeySaved(QString key)
{
    QSettings settings;
    QStringList keyList(settings.value("registerKey").toStringList());

    foreach(QString element, keyList)
    {
        if (element == key)
            return true;
    }
    return false;
}

void RegisterDialog::on_lineEdit_textChanged(QString key)
{
    if (verifyKey(key))
    {
        ui->label->setText("<font color=\"green\"> <b>key is valid</b></font>");
        if (!isKeySaved(key))
        {
            QSettings settings;
            QStringList keyList(settings.value("registerKey").toStringList());
            keyList.append(key);
            settings.setValue("registerKey",keyList);
        }
        ui->pushButton->setText("ok");
        this->registered = true;
    }
    else
    {
        ui->label->setText("<font color=\"red\"> <b>key is invalid</b></font>");
    }
}

bool RegisterDialog::verifyKey(QString key)
{
    foreach(QString element,this->accountList)
    {
        QCryptographicHash md5(QCryptographicHash::Md5);
        QCryptographicHash sha1(QCryptographicHash::Sha1);

        md5.addData("QtADB"+element.toAscii()+"qtadb");
        sha1.addData(md5.result());

        if (key == sha1.result().toHex())
        {
            return true;
        }
    }
    return false;
}

void RegisterDialog::on_pushButton_pressed()
{
    this->close();
}
