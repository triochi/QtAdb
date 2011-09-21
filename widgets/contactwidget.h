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


//#ifndef CONTACTWIDGET_H
//#define CONTACTWIDGET_H

//#include <QtGui>


//typedef struct contactList
//{
//    QStringList _id;
//    QStringList name;
//    QStringList display_name;
//    QStringList phonetic_name;
//    QStringList notes;
//    QStringList account_name;
//    QStringList account_type;
//    QStringList times_contacted;
//    QStringList last_time_contacted;
//    QStringList custom_ringtone;
//    QStringList send_to_voicemail;
//    QStringList starred;
//    QStringList primary_organization;
//    QStringList primary_email;
//    QStringList primary_phone;
//    QStringList number;
//    QStringList type;
//    QStringList label;
//    QStringList number_key;
//} ContactList;

//class ThreadContacts : public QThread
//{
//    Q_OBJECT
//public:
//    void run();
//    QString sdk;

//signals:
//    void finished(ContactList contactList);
//};

//namespace Ui {
//    class ContactWidget;
//}

//class ContactWidget : public QWidget
//{
//    Q_OBJECT

//public:
//    explicit ContactWidget(QWidget *parent = 0);
//    ~ContactWidget();

//protected:
//    void changeEvent(QEvent *e);

//private:
//    Ui::ContactWidget *ui;
//    ThreadContacts threadContacts;

//private slots:
//    void contactSelected(int);
//    void insertContacts(ContactList contactList);
//};

//#endif // CONTACTWIDGET_H
