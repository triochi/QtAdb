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


#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QtGui>
#include <QtNetwork>
#include <QDeclarativeView>
#include <QDeclarativeContext>

#include "../classes/models/messagethreadmodel.h"
#include "../classes/models/contactmodel.h"


namespace Ui {
    class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    MessageWidget(QWidget *parent, QString clientIP);
    ~MessageWidget();

    QString clientIP;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MessageWidget *ui;
    QDeclarativeView *qmlView;
    QDeclarativeContext *qmlViewCtx;

    MessageModel messageModel;
    MessageThreadModel messageThreadModel;
    ContactModel contactModel;
    QSortFilterProxyModel sortModel;
    QSortFilterProxyModel sortThreadModel;
    QSortFilterProxyModel sortContactModel;
    QString sdk;
    Message *previousMessage;
    int messageCount;
    void getSmsList();
    QTcpServer tcpServer;



private slots:
    void on_pushButton_pressed();
    void smsRead(QString threadId, QString messageId, QString timestamp, QString number, QString read, QString toa, QString body);
    void addSMS(QString threadId, QString messageId, QString timestamp, QString number, QString read, QString toa, QString body);
    void clearModels();
    void addContact(QString id, QString number, QString name);

public slots:
    void filterMessages(QString filter);
    void filterContacts(QString filter);
    void getContactList();
    void sendSmsThread(QString message);
    void sendSms(QString number,QString message);
    void smsReceivedSlot(QString number, QString body);
    void insertSmsToDatabase(QString number, QString body, QString timestamp, QString type);
    void smsResult(QString result);
    void markMessageAsRead(QString id);
    void markThreadAsRead(QString id);
    void connectToClient();
    void newConnection();
    void sendToClient(QString message);
    QString getDateFromTimestamp(QString timestamp);
    void updateThread(QString id, QString timestamp,QString lastBody, QString messageCount, QString read);

signals:
    void smsReceived(QString number, QString body);
    void smsResultSignal(QString result);

};

#endif // MESSAGEWIDGET_H
