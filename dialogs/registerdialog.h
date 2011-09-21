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


#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QCryptographicHash>
#include <QSettings>

namespace Ui {
    class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    RegisterDialog(QWidget *parent,QStringList accountList);
    ~RegisterDialog();
    bool registered;

private slots:
    void on_lineEdit_textChanged(QString );
    bool verifyKey(QString key);

    void on_pushButton_pressed();

private:
    Ui::RegisterDialog *ui;
    QStringList accountList;
};

#endif // REGISTERDIALOG_H
