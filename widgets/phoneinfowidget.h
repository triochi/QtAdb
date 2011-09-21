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


#ifndef PHONEINFOWIDGET_H
#define PHONEINFOWIDGET_H

#include <QtGui>
#include "filewidget.h"

namespace Ui {
    class PhoneInfoWidget;
}

class PhoneInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhoneInfoWidget(QWidget *parent = 0, QString serialNumber = "");
    ~PhoneInfoWidget();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButton_clicked();
    void showPhoneInfo();

private:
    Ui::PhoneInfoWidget *ui;
    QString sdk;
    QString humanReadableSize(QString size);
    QTimer timer;
};

#endif // PHONEINFOWIDGET_H
