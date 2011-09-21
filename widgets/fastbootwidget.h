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


#ifndef FASTBOOTWIDGET_H
#define FASTBOOTWIDGET_H

#include <QtGui>
#include "../classes/phone.h"

namespace Ui {
    class FastbootWidget;
}

class FastbootWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FastbootWidget(QWidget *parent = 0, Phone *phone = NULL);
    ~FastbootWidget();

    QString sdk;

//protected:
//    void changeEvent(QEvent *e);

private:
    Ui::FastbootWidget *ui;
    Phone *phone;

private slots:
    void flashSPL();
    void flashRecovery();
    void flashRadio();
    void bootIMG();
};

#endif // FASTBOOTWIDGET_H
