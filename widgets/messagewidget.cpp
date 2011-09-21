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


#include "messagewidget.h"
#include "ui_messagewidget.h"


MessageWidget::MessageWidget(QWidget *parent, QString clientIP) :
    QWidget(parent),
    ui(new Ui::MessageWidget)
{
    ui->setupUi(this);

    this->setLayout(ui->layoutMessages);

    this->sortModel.setSourceModel(&messageModel);
    this->sortModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortModel.setDynamicSortFilter(true);
    this->sortModel.setFilterRole(MessageModel::ThreadIdRole);
    this->sortModel.setSortRole(MessageModel::TimeStampRole);
    this->sortModel.sort(0, Qt::DescendingOrder);

    this->sortThreadModel.setSourceModel(&this->messageThreadModel);
    this->sortThreadModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortThreadModel.setDynamicSortFilter(true);
    this->sortThreadModel.setSortRole(MessageThreadModel::TimeStampRole);
    this->sortThreadModel.sort(0, Qt::DescendingOrder);

    this->sortContactModel.setSourceModel(&this->contactModel);
    this->sortContactModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    this->sortContactModel.setDynamicSortFilter(true);
    this->sortContactModel.setFilterRole(ContactModel::NameRole);
    this->sortContactModel.setSortRole(ContactModel::NameRole);
    this->sortContactModel.sort(0, Qt::AscendingOrder);

    this->messageCount=0;

    this->qmlView =  new QDeclarativeView;
    this->qmlViewCtx = this->qmlView->rootContext();
    if (QFile::exists("qml/messageView.qml"))
        this->qmlView->setSource(QUrl::fromLocalFile("qml/messageView.qml"));
    else
        //this->qmlView->setSource(QUrl::fromLocalFile("../messages/qml/messageView.qml"));
        this->qmlView->setSource(QUrl("qrc:/qml/qml/messageView.qml"));
    this->qmlViewCtx->setContextProperty("threadModel", &sortThreadModel);
    this->qmlViewCtx->setContextProperty("contactModel", &sortContactModel);
    this->qmlViewCtx->setContextProperty("messageModel", &sortModel);
    this->qmlViewCtx->setContextProperty("myNickname", "Me");
    this->qmlViewCtx->setContextProperty("contactDefault", "qrc:/icons/contactDefault.png");
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    this->qmlViewCtx->setContextProperty("messageWidget", this);

    ui->layoutMessages->addWidget(this->qmlView,1,1);

    //QSettings settings;
    //this->sdk = settings.value("sdkPath").toString();
    this->tcpServer.listen(QHostAddress::Any,4445);

    connect(&this->tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    this->clientIP = clientIP;

    on_pushButton_pressed();
}

MessageWidget::~MessageWidget()
{

    delete ui;
}

void MessageWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MessageWidget::clearModels()
{
    this->messageCount = 0;
    this->messageModel.clear();
    this->messageThreadModel.clear();
    this->contactModel.clear();
    //    this->qmlView->update();
}

void MessageWidget::updateThread(QString id, QString timestamp,QString lastBody, QString messageCount, QString read)
{
    this->messageThreadModel.updateThread(id,timestamp,lastBody,messageCount,read);
}

void MessageWidget::addContact(QString id, QString number, QString name)
{
    this->contactModel.addContact(Contact(id,number,name));
}

void MessageWidget::addSMS(QString threadId, QString messageId, QString timestamp, QString number, QString read, QString toa, QString body)
{
    QString contactName = this->contactModel.getName(number);
    this->messageCount++;
    if (!this->messageThreadModel.exists(threadId))
    {
        this->messageCount = 1;
        this->messageThreadModel.addThread(MessageThread(threadId,number,timestamp,read,body,QString::number(this->messageCount),contactName));
    }
    else
    {
        updateThread(threadId, timestamp ,body, QString::number(this->messageCount), read);
    }
    this->messageModel.addMessage(Message(threadId, messageId, timestamp, number, read, toa, body, contactName));
}

void MessageWidget::smsRead(QString threadId, QString messageId, QString timestamp, QString number, QString read, QString toa, QString body)
{
    QString contactName = this->contactModel.getName(number);
    QString tmp;

    this->messageCount++;
    if ((threadId == "") && (messageId == "") && (timestamp == "") && (number == "") && (read == "") && (toa == "") && (body == ""))
    {

        this->messageThreadModel.addThread(MessageThread(previousMessage->getThreadId(),previousMessage->getNumber(),previousMessage->getTimeStamp(),
                                                         previousMessage->getRead(),previousMessage->getBody(),QString::number(this->messageCount),previousMessage->getContactName()));
        return;
    }

    if (previousMessage != NULL)
        if (this->previousMessage->getThreadId() != threadId)
        {

            this->messageThreadModel.addThread(MessageThread(previousMessage->getThreadId(),previousMessage->getNumber(),previousMessage->getTimeStamp(),
                                                             previousMessage->getRead(),previousMessage->getBody(),QString::number(this->messageCount),previousMessage->getContactName()));
            this->messageCount=0;
        }

    this->messageModel.addMessage(Message(threadId, messageId, timestamp, number, read, toa, body, contactName));
    if (this->previousMessage != NULL)
        delete this->previousMessage;
    this->previousMessage=new Message(threadId,messageId,timestamp,number,read,toa,body,contactName);
}

void MessageWidget::newConnection()
{
    QTcpSocket *socket = this->tcpServer.nextPendingConnection();
    if (socket->waitForReadyRead(2000))
    {
        QString output= QString::fromUtf8(socket->readAll());


        if (output.contains(QRegExp("^SMS_RECEIVED:")))
        {//"SMS_RECEIVED:number:body
            output.remove(QRegExp("^SMS_RECEIVED:"));

            int i;
            QString number,body;

            i = output.indexOf(":");
            number = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            body = output;

            emit this->smsReceived(this->contactModel.getName(number),body);
        }
        else if (output.contains(QRegExp("^GET_SMS_LIST:")))
        {//"GET_SMS_LIST:threadId:messageId:timestamp:number:read:toa:body
            output.remove(QRegExp("^GET_SMS_LIST:"));
            int i;
            QString threadId,messageId,timestamp,number,read,toa,body;

            i = output.indexOf(":");
            threadId = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            messageId = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            timestamp = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            number = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            read = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            toa = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            body = output;

            addSMS(threadId,messageId,timestamp,number,read,toa,body);
        }
        else if (output.contains(QRegExp("^GET_CONTACT_LIST:")))
        {
            output.remove(QRegExp("^GET_CONTACT_LIST:"));
            QString id,number,name;
            int i;

            i = output.indexOf(":");
            id = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            name = output.left(i);
            output.remove(0,i+1);

            i = output.indexOf(":");
            number = output;
            number.remove("\n");

            addContact(id,number,name);
        }

        socket->disconnectFromHost();
    }
    delete socket;
}

void MessageWidget::sendToClient(QString message)
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost(this->clientIP,4444,QTcpSocket::ReadWrite);
    if (socket->waitForConnected(2000))
    {
        socket->write(message.toLatin1());
        socket->waitForReadyRead(300);
        socket->disconnectFromHost();
    }
    else
        QMessageBox::warning(this,"connection problem", "connection to client failed", QMessageBox::Close);
    delete socket;
}

void MessageWidget::connectToClient()
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost(this->clientIP,4444,QTcpSocket::ReadWrite);
    if (!socket->waitForConnected(2000))
    {
        QMessageBox::warning(this,"connection problem", "connection to client failed", QMessageBox::Close);
    }
    delete socket;
}

void MessageWidget::on_pushButton_pressed()
{
    clearModels();
    getContactList();
    getSmsList();
}

void MessageWidget::getSmsList()
{
    sendToClient("GET_SMS_LIST");
}

void MessageWidget::getContactList()
{
    sendToClient("GET_CONTACT_LIST");
}

void MessageWidget::markMessageAsRead(QString id)
{
    sendToClient("MARK_MESSAGE_AS_READ:"+id.toLatin1());
}

void MessageWidget::markThreadAsRead(QString id)
{
    sendToClient("MARK_THREAD_AS_READ:"+id.toLatin1());
    this->messageThreadModel.markAsRead(id);
    this->messageModel.markThreadAsRead(id);
}

void MessageWidget::insertSmsToDatabase(QString number, QString body, QString timestamp, QString type)
{
    sendToClient("INSERT_SMS:"+number.toLatin1()+":"+body.toLatin1()+":"+timestamp.toLatin1()+":"+type.toLatin1());
}

void MessageWidget::sendSmsThread(QString message)
{

    MessageThread thread = this->messageThreadModel.getThread(this->sortModel.filterRegExp().pattern());
    QString number = thread.getNumber();

    if (number == "")
        return;
    sendToClient("SEND_SMS:"+number.toLatin1()+":"+message.toLatin1());

    addSMS(thread.getId(), "1",QString::number(QDateTime::currentMSecsSinceEpoch()),number,"1","outbox",message);
}

void MessageWidget::sendSms(QString number,QString message)
{
    if (number.contains('('))
    {
        number.chop(1);
        number = number.mid(number.indexOf("(")+1,number.length());
    }
    sendToClient("SEND_SMS:"+number.toLatin1()+":"+message.toLatin1());

    //addSMS(, "1",QString::number(QDateTime::currentMSecsSinceEpoch()),number,"1","outbox",message);
}

void MessageWidget::filterMessages(QString filter)
{
    //QString pattern;
    //pattern = /*".*"+*/filter/*+".*"*/;
    //QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::FixedString);
    //this->sortModel.setFilterRegExp(regExp);
    //qDebug()<<"filterMessages: "+filter.toLatin1();
    //this->sortModel.setFilterFixedString(filter);
    this->sortModel.setFilterRegExp(QRegExp("^"+filter+"$"));
}

void MessageWidget::filterContacts(QString filter)
{
    QString pattern;
    pattern = /*".*"+*/filter/*+".*"*/;
    QRegExp regExp(pattern, Qt::CaseInsensitive, QRegExp::FixedString);
    this->sortContactModel.setFilterRegExp(regExp);
}

void MessageWidget::smsReceivedSlot(QString number, QString body)
{

}

void MessageWidget::smsResult(QString result)
{
    emit this->smsResultSignal(result);
}

QString MessageWidget::getDateFromTimestamp(QString timestamp)
{
    return QDateTime::fromMSecsSinceEpoch(timestamp.toLongLong()).toString("hh:mm dd.MM.yyyy");
}

