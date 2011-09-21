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


//#include "contactwidget.h"
//#include "ui_contactwidget.h"

//ContactWidget::ContactWidget(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::ContactWidget)
//{
//    ui->setupUi(this);

//    connect(ui->listWidgetContacts, SIGNAL(currentRowChanged(int)), this, SLOT(contactSelected(int)));
//    connect(&this->threadContacts, SIGNAL(finished(ContactList)), this, SLOT(insertContacts(ContactList)));

//    ui->pageContacts->setLayout(ui->layoutContacts);
//}

//ContactWidget::~ContactWidget()
//{
//    delete ui;
//}

//void ContactWidget::changeEvent(QEvent *e)
//{
//    QWidget::changeEvent(e);
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
//        break;
//    default:
//        break;
//    }
//}

//void ContactWidget::insertContacts(ContactList contactList)
//{
//    ui->listWidgetContacts->clear();
//    if (this->contactList._id.length() == 0 || (this->contactList._id.length() < contactList._id.length()))
//        this->contactList=contactList;

//    for (int i=0;i<contactList._id.count();i++)
//    {
//        ui->listWidgetContacts->insertItem(ui->listWidgetContacts->count(),contactList.display_name.at(i));
//    }
//}

//void ContactWidget::contactSelected(int row)
//{
//    ui->lineEditContactsAccountName->setText(contactList.account_name.at(row));
//    ui->lineEditContactsAccountType->setText(contactList.account_type.at(row));
//    ui->lineEditContactsCustomeRingthone->setText(contactList.custom_ringtone.at(row));
//    ui->lineEditContactsDisplayName->setText(contactList.display_name.at(row));
//    ui->lineEditContactsId->setText(contactList._id.at(row));
//    ui->lineEditContactsLabel->setText(contactList.label.at(row));
//    ui->lineEditContactsLastTimeContacted->setText(contactList.last_time_contacted.at(row));
//    ui->lineEditContactsName->setText(contactList.name.at(row));
//    ui->lineEditContactsNotes->setText(contactList.notes.at(row));
//    ui->lineEditContactsNumber->setText(contactList.number.at(row));
//    ui->lineEditContactsNumberKey->setText(contactList.number_key.at(row));
//    ui->lineEditContactsPhoneticName->setText(contactList.phonetic_name.at(row));
//    ui->lineEditContactsPrimaryEmail->setText(contactList.primary_email.at(row));
//    ui->lineEditContactsPrimaryOrganization->setText(contactList.primary_organization.at(row));
//    ui->lineEditContactsPrimaryPhone->setText(contactList.primary_phone.at(row));
//    ui->lineEditContactsSendToVoicemail->setText(contactList.send_to_voicemail.at(row));
//    ui->lineEditContactsStarred->setText(contactList.starred.at(row));
//    ui->lineEditContactsTimesContacted->setText(contactList.times_contacted.at(row));
//    ui->lineEditContactsType->setText(contactList.type.at(row));
//}

//void ThreadContacts::run()
//{
//    QString tmp;
//    QProcess *proces=new QProcess;
//    proces->start("\""+sdk+"\""+"adb shell sqlite3");
//    proces->waitForReadyRead(-1);
//    tmp=proces->readLine();
//    proces->write(QString(QChar(0x3)).toAscii());
//    proces->close();

//    if (tmp.contains("not found"))
//    {
//        delete proces;
////        emit this->finished();
//        return;
//    }

//    ContactList contactList;
//    QStringList tmpList;

//    proces->start("\""+sdk+"\""+"adb shell sqlite3 /data/data/com.android.providers.contacts/databases/contacts2.db \"select * from view_v1_people where number IS NOT NULL;;\"");
//    while (true)
//    {
//        proces->waitForReadyRead(-1);
//        tmp=proces->readLine();
//        if (tmp.isEmpty())
//            break;
//        tmpList=tmp.split("|");

//        contactList._id.append(tmpList.takeFirst());
//        contactList.name.append(tmpList.takeFirst());
//        contactList.display_name.append(tmpList.takeFirst());
//        contactList.phonetic_name.append(tmpList.takeFirst());
//        contactList.notes.append(tmpList.takeFirst());
//        contactList.account_name.append(tmpList.takeFirst());
//        contactList.account_type.append(tmpList.takeFirst());
//        contactList.times_contacted.append(tmpList.takeFirst());
//        contactList.last_time_contacted.append(tmpList.takeFirst());
//        contactList.custom_ringtone.append(tmpList.takeFirst());
//        contactList.send_to_voicemail.append(tmpList.takeFirst());
//        contactList.starred.append(tmpList.takeFirst());
//        contactList.primary_organization.append(tmpList.takeFirst());
//        contactList.primary_email.append(tmpList.takeFirst());
//        contactList.primary_phone.append(tmpList.takeFirst());
//        contactList.number.append(tmpList.takeFirst());
//        contactList.type.append(tmpList.takeFirst());
//        contactList.label.append(tmpList.takeFirst());
//        contactList.number_key.append(tmpList.takeFirst());
//    }
//    proces->close();
//    delete proces;
//    emit this->finished(contactList);
//}
