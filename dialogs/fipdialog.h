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


#ifndef FIPDIALOG_H
#define FIPDIALOG_H

#include <QDialog>
#include <QString>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QTextCodec>

namespace Ui {
    class fipDialog;
}



class fipDialog: public QDialog
{
    Q_OBJECT

public:
  fipDialog(QWidget *parent);
  void setData(QString file);
  ~fipDialog();
  QTextCodec *codec;

private:
    Ui::fipDialog *ui;
    QString getCheckboxesStatus();
    QString  fileC;
    QProcess *proces;
    QSettings settings;
    QString sdk;
    QString  fChmod;

protected slots:
  void applyButtonClicked();
  void cancelButtonClicked();
  void okButtonClicked();
  void statusCnanged();

private slots:

signals:

};




#endif // FIPDIALOG_H
