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


#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    this->setLayout(this->gridLayout);
    QString version = QCoreApplication::applicationVersion();
    this->versionLabel->setText(version);

    QSettings settings;
    QFont font;

    font.setFamily(settings.value("fontAppFamily").toString());
    font.setPointSize(settings.value("fontAppSize").toInt());
    this->label_4->setFont(font);
    this->label_5->setFont(font);
    this->label_6->setFont(font);
    this->label_7->setFont(font);
    this->label_8->setFont(font);
    this->label_9->setFont(font);
    this->label_10->setFont(font);
    this->okButton->setFont(font);
    this->versionLabel->setFont(font);
    this->label->setFont(font);
    this->label_13->setFont(font);
    this->label_14->setFont(font);

    connect(this->okButton, SIGNAL(clicked()), this, SLOT(close()));
}
