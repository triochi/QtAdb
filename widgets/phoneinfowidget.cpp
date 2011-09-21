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


#include "phoneinfowidget.h"
#include "ui_phoneinfowidget.h"

QString PhoneInfoWidget::humanReadableSize(QString size)
{
    long double sizeTmp = 0.0;
    QString strTmp = " B";
    sizeTmp = size.toLongLong();
    if (sizeTmp <= 1024)
    {
        strTmp.prepend(QString::number(sizeTmp, 'f', 0));
        return strTmp;
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " KiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " MiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " GiB";
    }
    strTmp.prepend(QString::number(sizeTmp, 'f', 2));
    return strTmp;
}

PhoneInfoWidget::PhoneInfoWidget(QWidget *parent, QString serialNumber) :
    QWidget(parent),
    ui(new Ui::PhoneInfoWidget)
{
    ui->setupUi(this);

    this->setLayout(ui->layoutPhoneInfo);
    ui->lineEditSerialNumber->setText(serialNumber);

    QSettings settings;
    this->sdk=settings.value("sdkPath").toString();
    this->timer.start(60000);

    connect(&this->timer, SIGNAL(timeout()), this , SLOT(showPhoneInfo()));
    showPhoneInfo();
}

PhoneInfoWidget::~PhoneInfoWidget()
{
    delete ui;
}

void PhoneInfoWidget::changeEvent(QEvent *e)
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

void PhoneInfoWidget::showPhoneInfo()
{
        QString data,tmp="1";
        QStringList list;
        QProcess *proces=new QProcess;
        proces->start("\""+sdk+"\""+"adb shell getprop");
        while (!tmp.isEmpty())
        {
            proces->waitForReadyRead(-1);
            tmp=proces->readLine();
            list.append(tmp);
        }

        for (int i=0;i<list.length();i++)
        {
            qDebug()<<"Getprop - "<<list.at(i);
            if (list.at(i).contains("[ro.product.model]"))
            {
                tmp=list.at(i);
                tmp.remove("[ro.product.model]: ");
                tmp.remove("[");
                tmp.remove("]");
                ui->lineEditModel->setText(tmp);
            }
            else if (list.at(i).contains("[ro.baseband]"))
            {
                tmp=list.at(i);
                tmp.remove("[ro.baseband]: ");
                tmp.remove("[");
                tmp.remove("]");
                ui->lineEditRadio->setText(tmp);
            }
            else if (list.at(i).contains("[ro.bootloader]"))
            {
                tmp=list.at(i);
                tmp.remove("[ro.bootloader]: ");
                tmp.remove("[");
                tmp.remove("]");
                ui->lineEditBootloader->setText(tmp);
            }
            else if (list.at(i).contains("[ro.build.display.id]"))
            {
                tmp=list.at(i);
                tmp.remove("[ro.build.display.id]: ");
                tmp.remove("[");
                tmp.remove("]");
                ui->lineEditRom->setText(tmp);
            }
            else if (list.at(i).contains("[gsm.operator.alpha]"))
            {
                tmp=list.at(i);
                tmp.remove("[gsm.operator.alpha]: ");
                tmp.remove("[");
                tmp.remove("]");
                ui->lineEditOperator->setText(tmp);
            }
        }
//        ui->lineEditSerialNumber->setText(this->phone->serialNumber);



        proces->start("\""+sdk+"\""+"adb shell busybox cat /sys/class/power_supply/battery/capacity");
        proces->waitForReadyRead(-1);
        tmp=proces->readLine();
        ui->progressBarBatteryLevel->setValue(tmp.toInt());
        proces->close();
        //get sd-ext folder
        QString sdFolder;
//        QStringList lines, split;
//        sdFolder.clear();
//        proces->start("\"" + this->sdk + "\"adb shell busybox mount");
//        proces->waitForFinished(-1);
//        tmp = proces->readAll();
//        qDebug()<<"Get phone info mount - "<<tmp;
//        if (tmp.contains("ext"))
//        {
//            lines = tmp.split("\n", QString::SkipEmptyParts);
//            while (lines.size() > 0)
//            {
//                tmp = lines.takeFirst();
//                if (tmp.contains("ext"))
//                {
//                    split = tmp.split(QRegExp("\\s+"),QString::SkipEmptyParts);
//                    if (split.size() > 2)
//                    {
//                        sdFolder = split.at(2);
//                        if (sdFolder.endsWith("/",Qt::CaseInsensitive))
//                            sdFolder.remove(sdFolder.length() - 1, 1);
//                    }
//                }
//            }
//        }
        QSettings settings;
        sdFolder = settings.value("sdFolder").toString();
        if (!sdFolder.isEmpty() && !sdFolder.contains(QRegExp("<.*>")))
        {
            if (sdFolder.endsWith("/",Qt::CaseInsensitive))
                sdFolder.chop(1);
        }
        proces->start("\""+sdk+"\""+"adb shell busybox df");
        tmp.clear();

        while (true)
        {
            proces->waitForReadyRead(-1);
            data=proces->readLine();
            if (data.isEmpty())
                break;
            tmp.append(data);
        }
        if (!tmp.contains(sdFolder) || sdFolder.isEmpty())
        {
            ui->progressBarExt->setDisabled(true);
            ui->labelExtAvailable->setDisabled(true);
            ui->labelExt->setDisabled(true);
            ui->labelExtSize->setDisabled(true);
            ui->labelExtUsed->setDisabled(true);
            ui->lineEditExtAvailable->setDisabled(true);
            ui->lineEditExtSize->setDisabled(true);
            ui->lineEditExtUsed->setDisabled(true);
        }
        list=tmp.split("\n");
        QStringList parts;
        QString used,available;
        while (list.count()>0)
        {
            tmp=list.takeFirst();
            qDebug()<<"df - "<<tmp;
            parts=tmp.split(QRegExp("\\s+"));
            if (parts.size()>2)
            {
                parts.removeLast();
                tmp=parts.takeLast();
                parts.removeLast();
                available=parts.takeLast();
                used=parts.takeLast();
            }

            if (tmp=="/data")
            {
                ui->lineEditDataAvailable->setText(PhoneInfoWidget::humanReadableSize(available+"000"));
                ui->lineEditDataUsed->setText(PhoneInfoWidget::humanReadableSize(used+"000"));
                ui->lineEditDataSize->setText(PhoneInfoWidget::humanReadableSize(QString::number(used.toUInt()+available.toUInt())+"000"));
                ui->progressBarData->setMaximum(used.toUInt()+available.toUInt());
                ui->progressBarData->setValue(used.toUInt());
            }
            else if (tmp=="/system")
            {
                ui->lineEditSystemAvailable->setText(PhoneInfoWidget::humanReadableSize(available+"000"));
                ui->lineEditSystemUsed->setText(PhoneInfoWidget::humanReadableSize(used+"000"));
                ui->lineEditSystemSize->setText(PhoneInfoWidget::humanReadableSize(QString::number(used.toUInt()+available.toUInt())+"000"));
                ui->progressBarSystem->setMaximum(used.toUInt()+available.toUInt());
                ui->progressBarSystem->setValue(used.toUInt());
            }
            else if (tmp.contains(sdFolder) && !sdFolder.isEmpty())
            {
                ui->lineEditExtAvailable->setText(PhoneInfoWidget::humanReadableSize(available+"000"));
                ui->lineEditExtUsed->setText(PhoneInfoWidget::humanReadableSize(used+"000"));
                ui->lineEditExtSize->setText(PhoneInfoWidget::humanReadableSize(QString::number(used.toUInt()+available.toUInt())+"000"));
                ui->progressBarExt->setMaximum(used.toUInt()+available.toUInt());
                ui->progressBarExt->setValue(used.toUInt());
            }
            else if (tmp.contains("/sdcard"))
            {
                ui->lineEditSdcardAvailable->setText(PhoneInfoWidget::humanReadableSize(available+"000"));
                ui->lineEditSdcardUsed->setText(PhoneInfoWidget::humanReadableSize(used+"000"));
                ui->lineEditSdcardSize->setText(PhoneInfoWidget::humanReadableSize(QString::number(used.toUInt()+available.toUInt())+"000"));
                ui->progressBarSdcard->setMaximum(used.toUInt()+available.toUInt());
                ui->progressBarSdcard->setValue(used.toUInt());
            }
        }
//        int i=0;
        //df
}

void PhoneInfoWidget::on_pushButton_clicked()
{
    showPhoneInfo();
}
