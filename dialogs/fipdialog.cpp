/***********************************************************************
*Copyright 2010-20XX by nijel
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
*   @author nijel (nijel8@gmail.com)
*
************************************************************************/


#include "fipdialog.h"
#include "ui_fipdialog.h"
#include <QTextCodec>

fipDialog::fipDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fipDialog)
{
   ui->setupUi(this);
   this->setFixedSize(this->width(),this->height());
   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
   this->codec = QTextCodec::codecForLocale();

   proces = new QProcess;
   sdk=settings.value("sdkPath").toString();

   connect(this->ui->buttonCancel, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));

}

void fipDialog::statusCnanged()
{
    QString chmod = getCheckboxesStatus();
    //qDebug()<<"statusCnanged, chmod ="<<chmod;
        if (chmod.isEmpty())
        {
           disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
           disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
           this->ui->buttonApply->setText("Apply");
           this->ui->buttonApply->setDisabled(true);
        }
        else
        {
           if (chmod == fChmod)
           {
              disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
              disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
              this->ui->buttonApply->setText("Apply");
              this->ui->buttonApply->setDisabled(true);
           }
           else
           {
              disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
              connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
              this->ui->buttonApply->setEnabled(true);
              this->ui->buttonApply->setText("Apply");
           }
       }
}



fipDialog::~fipDialog()
{
    proces->close();
    proces->terminate();
    delete proces;
    delete ui;
}

void fipDialog::setData(QString file)
{
    fileC = file;
    if (!file.isEmpty())
    {
        QString command="\""+sdk+"\""+"adb shell stat \""+this->codec->toUnicode(fileC.toUtf8())+"\"";
        proces->start(command);
        proces->waitForFinished(-1);
        QString output = proces->readAll();
        qDebug()<<"\""<<sdk<<"\""<<"adb shell stat \""<<this->codec->toUnicode(fileC.toUtf8())<<"\"";
        qDebug()<<"command output "<<output;
        if (output.contains("stat:") || output.contains("can't stat") || output.contains("error:"))
        {
            QMessageBox::critical(0,"stat ERROR:",output);
            this->deleteLater();
        }
        else
        {
        QString fileLocation = file.left(file.lastIndexOf("/")+1);
        QString fileName = file.remove (fileLocation , Qt::CaseSensitive );
        this->ui->editLocation->setText(fileLocation);
        this->ui->editName->setText(fileName);
        if (output.contains("directory",Qt::CaseSensitive))
        {
            this->ui->editType->setText("Directory");
          //  QProcess *procesdir = new QProcess;
            QString commandDir="\""+sdk+"\""+"adb shell du -H -s \""+this->codec->toUnicode(fileC.toUtf8())+"\"";
            proces->start(commandDir);
            proces->waitForFinished(-1);
            QString outputDir = proces->readAll();
            qDebug()<<"\""<<sdk<<"\""<<"adb shell du -H -s \""<<this->codec->toUnicode(fileC.toUtf8())<<"\"";
            qDebug()<<"du outputDir" << outputDir;
            if (outputDir.contains("du:") || outputDir.contains("error:"))
            {
                QMessageBox::critical(0,"du ERROR:",outputDir);
                this->deleteLater();
            }
            outputDir = outputDir.left(outputDir.indexOf("/"));
            outputDir = outputDir.trimmed();
            long double sizeTmp = 0.0;
            QString strTmp = " KB";
            sizeTmp = outputDir.toLongLong();
            if (sizeTmp <= 1024)
            {
               strTmp.prepend(QString::number(sizeTmp, 'f', 0));
               this->ui->editSize->setText(strTmp);
               strTmp = "";
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
            if (sizeTmp > 1024)
            {
                sizeTmp = sizeTmp / 1024;
                strTmp = " TB";
            }
            if (!strTmp.isEmpty())
            {
                strTmp.prepend(QString::number(sizeTmp, 'f', 3));
                this->ui->editSize->setText(strTmp);
            }
            if (output.contains("symbolic link",Qt::CaseSensitive))
                this->ui->editType->setText("Symbolic Link");
            this->ui->editLink->setDisabled(true);
        }
        else
        {
            if (output.contains("symbolic link",Qt::CaseSensitive))
            {
                int start = output.indexOf("->",Qt::CaseSensitive);
                start+=2;
                int end = output.indexOf("Size:",start,Qt::CaseSensitive);
                QString fLinkto = output.mid(start, end-start);
                fLinkto = fLinkto.trimmed(); //'/sys/kernel/debug'
                fLinkto = fLinkto.remove("'");
                this->ui->editLink->setText(fLinkto);
                this->ui->editType->setText("Symbolic Link");

            }
            else if (output.contains("regular file",Qt::CaseSensitive) || output.contains("regular empty file",Qt::CaseSensitive))
            {
                if (!fileName.contains("."))
                {
                this->ui->editType->setText("File");
                }
                else
                {
                QString noExt = fileName.left(fileName.lastIndexOf(".")+1);
                this->ui->editType->setText(fileName.remove(noExt , Qt::CaseSensitive ).toUpper() + " File");
                }
                this->ui->editLink->setDisabled(true);
            }

            else
            {
                this->ui->editType->setText("Device");
                this->ui->editLink->setDisabled(true);
            }

            int start = output.indexOf("Size:",Qt::CaseSensitive);
            start+=5;
            int end = output.indexOf("Blocks:",start,Qt::CaseSensitive);
            QString fSize = output.mid(start, end-start);
            fSize = fSize.trimmed();
            long double sizeTmp = 0.0;
            QString strTmp = " B";
            sizeTmp = fSize.toLongLong();
            if (sizeTmp <= 1024)
            {
               strTmp.prepend(QString::number(sizeTmp, 'f', 0));
               this->ui->editSize->setText(strTmp);
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
                this->ui->editSize->setText(strTmp);
            }
        }
        int start = output.indexOf("Modify:",Qt::CaseSensitive);
        start+=7;
        int end = output.indexOf("Change:",start,Qt::CaseSensitive);
        QString fModified = output.mid(start, end-start);
        fModified = fModified.trimmed(); //1969-12-31 19:00:00.000000000
        fModified = fModified.left(fModified.indexOf(".")); //1969-12-31 19:00:00
        QString year = fModified.left(fModified.indexOf("-"));
        fModified.remove(year + "-");
        fModified.replace("-","/");
        fModified.insert(5,"/" + year);
        this->ui->editModified->setText(fModified);

        start = output.indexOf("Uid:",Qt::CaseSensitive);
        start+=4;
        end = output.indexOf("Gid:",start,Qt::CaseSensitive);
        QString fOwner = output.mid(start, end-start);
        fOwner = fOwner.trimmed(); //(    0/    root)
        fOwner = fOwner.right(fOwner.length()-fOwner.lastIndexOf("/")-1);
        fOwner = fOwner.trimmed();
        fOwner.remove(")");
        this->ui->editOwner->setText(fOwner);

        start = output.indexOf("Gid:",Qt::CaseSensitive);
        start+=4;
        end = output.indexOf("Access:",start,Qt::CaseSensitive);
        QString fGroup = output.mid(start, end-start);
        fGroup = fGroup.trimmed(); //(    0/    root)
        fGroup = fGroup.right(fGroup.length()-fGroup.lastIndexOf("/")-1);
        fGroup = fGroup.trimmed();
        fGroup.remove(")");
        this->ui->editGroup->setText(fGroup);

        start = output.indexOf("Access:",Qt::CaseSensitive);
        start+=7;
        end = output.indexOf("Uid:",start,Qt::CaseSensitive);
        QString fPerm = output.mid(start, end-start);
        fPerm = fPerm.trimmed(); //(0777/lrwxrwxrwx)
        QStringList permList = fPerm.split("/");
        QString rwx = permList[1].mid(1,9);
        fChmod = permList[0].right(3);
        this->ui->l_editChmod->setText(fChmod);
        this->ui->l_editRWEx->setText(rwx);

        QString tmp;
        if ((tmp = rwx.at(0)) != "-")
            this->ui->checkUr->setChecked(true);
        if ((tmp = rwx.at(1)) != "-")
            this->ui->checkUw->setChecked(true);
        if ((tmp = rwx.at(2)) != "-")
            this->ui->checkUx->setChecked(true);
        if ((tmp = rwx.at(3)) != "-")
            this->ui->checkGr->setChecked(true);
        if ((tmp = rwx.at(4)) != "-")
            this->ui->checkGw->setChecked(true);
        if ((tmp = rwx.at(5)) != "-")
            this->ui->checkGx->setChecked(true);
        if ((tmp = rwx.at(6)) != "-")
            this->ui->checkOr->setChecked(true);
        if ((tmp = rwx.at(7)) != "-")
            this->ui->checkOw->setChecked(true);
        if ((tmp = rwx.at(8)) != "-")
            this->ui->checkOx->setChecked(true);
    }

    connect(ui->checkOx, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkUx, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkGx, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkOw, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkUw, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkGw, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkOr, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkUr, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));
    connect(ui->checkGr, SIGNAL(toggled(bool)), this, SLOT(statusCnanged()));

    this->ui->buttonApply->setDisabled(true);
    }
}



QString fipDialog::getCheckboxesStatus()
{
    QString chmod = "";
    QString u = "";
    QString g = "";
    QString o = "";
    QString tmp = "";
    QString ul = "";
    QString gl = "";
    QString ol = "";

     if (ui->checkUr->isChecked())
     {
         u.setNum(1);
         ul = "r";
     }
     else
     {
         u.setNum(0);
         ul = "-";
     }
     if (ui->checkUw->isChecked())
     {
        tmp.setNum(1);
        u.append(tmp);
        ul.append("w");
     }
     else
     {
        tmp.setNum(0);
        u.append(tmp);
        ul.append("-");
     }
     if (ui->checkUx->isChecked())
     {
         tmp.setNum(1);
        u.append(tmp);
        ul.append("x");
     }
     else
     {
        tmp.setNum(0);
       u.append(tmp);
       ul.append("-");
     }

     if (ui->checkGr->isChecked())
     {
         g.setNum(1);
         gl = "r";
     }
     else
     {
         g.setNum(0);
         gl = "-";
     }
     if (ui->checkGw->isChecked())
     {
         tmp.setNum(1);
        g.append(tmp);
        gl.append("w");
     }
     else
     {
         tmp.setNum(0);
       g.append(tmp);
       gl.append("-");
     }

     if (ui->checkGx->isChecked())
     {
         tmp.setNum(1);
        g.append(tmp);
        gl.append("x");
     }
     else
     {
         tmp.setNum(0);
        g.append(tmp);
        gl.append("-");
     }

     if (ui->checkOr->isChecked())
     {
        o.setNum(1);
        ol = "r";
     }
     else
     {
         o.setNum(0);
         ol = "-";
     }

     if (ui->checkOw->isChecked())
     {
         tmp.setNum(1);
        o.append(tmp);
        ol.append("w");

     }
     else
     {
         tmp.setNum(0);
        o.append(tmp);
        ol.append("-");
     }

     if (ui->checkOx->isChecked())
     {
         tmp.setNum(1);
        o.append(tmp);
        ol.append("x");
     }
     else
     {
         tmp.setNum(0);
        o.append(tmp);
        ol.append("-");
     }

     QStringList listBin = (QStringList() << "000" << "001" << "010" << "011" << "100" << "101" << "110" << "111" );
     //qDebug()<<"listBin ="<<listBin;
    chmod = tmp.setNum(listBin.indexOf(u));
    chmod.append(tmp.setNum(listBin.indexOf(g)));
    chmod.append(tmp.setNum(listBin.indexOf(o)));
    this->ui->l_editChmod->setText(chmod);
    this->ui->l_editRWEx->setText(ul+gl+ol);
    return chmod;
}

void fipDialog::cancelButtonClicked()
{
    this->close();
}

void fipDialog::applyButtonClicked()
{
    connect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    ui->buttonApply->setText("OK");
//    QString chmod = this->getCheckboxesStatus();
//    if (chmod != fChmod && !chmod.isEmpty())
//    {
//        qDebug()<<"Permissins before:----fChmod ="<<fChmod;
//        qDebug()<<"Permissions after:----chmod ="<<chmod;
//    }
//    else
//        qDebug()<<"Permissions won't change, chmod is empty or same!!! ="<<fChmod<<"=="<<chmod;
}

void fipDialog::okButtonClicked()
{
    disconnect(ui->buttonApply, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    QString chmod = this->getCheckboxesStatus();
    if (fileC.startsWith("/system"))
    {
        proces->start("\""+sdk+"\""+"adb shell mount -o remount,rw /system");
        proces->waitForFinished(-1);
    }
    QString commandChmod="\""+sdk+"\"" + "adb shell chmod " + chmod.append(" \"")+this->codec->toUnicode(fileC.toUtf8())+"\"";
    proces->start(commandChmod);
    proces->waitForFinished(-1);
    QString outputChmod = proces->readAll();
    qDebug()<<commandChmod;
    qDebug()<<"command output "<<outputChmod;
    if (outputChmod.contains("chmod:") || outputChmod.contains("error:") || outputChmod.contains("Usage:"))
    {
        QMessageBox::critical(0,"chmod ERROR:",outputChmod);
        this->deleteLater();
    }
    if (fileC.startsWith("/system"))
    {
        proces->start("\""+sdk+"\""+"adb shell mount -o remount,ro,noatime /system");
        proces->waitForFinished(-1);
    }
    this->close();
}
