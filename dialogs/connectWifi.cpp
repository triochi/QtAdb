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


#include "connectWifi.h"

ConnectWifi::ConnectWifi( QWidget * parent, Qt::WFlags f)
        : QDialog(parent, f)
{
        setupUi(this);
        this->setLayout(this->gridLayout);
        this->setBaseSize(220,100);
        this->setFixedSize(this->width(),this->height());
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->polaczButton->setDefault(true);
        connect(polaczButton,SIGNAL(clicked()),this,SLOT(accept()));
        connect(zamknijButton,SIGNAL(clicked()),this,SLOT(reject()));
}
