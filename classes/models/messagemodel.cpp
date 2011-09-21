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


#include "messagemodel.h"

Message::Message( const QString threadId, const QString messageId, const QString timeStamp, const QString number,
                 const QString read, const QString toa, const QString body, const QString contactName)
{
    this->body=body;
    this->messageId=messageId;
    this->number=number;
    this->threadId=threadId;
    this->timeStamp=timeStamp;
    this->read=read;
    this->toa=toa;
    this->contactName=contactName;
}

Message::Message()
{

}

QString Message::getThreadId() const
{
    return this->threadId;
}

QString Message::getMessageId() const
{
    return this->messageId;
}

QString Message::getTimeStamp() const
{
    return this->timeStamp;
}

QString Message::getNumber() const
{
    return this->number;
}

QString Message::getBody() const
{
    return this->body;
}

QString Message::getRead() const
{
    return this->read;
}

QString Message::getToa() const
{
    return this->toa;
}

QString Message::getContactName() const
{
    return this->contactName;
}

bool MessageModel::isMessageExists(QString id)
{
    foreach(Message message, this->messages)
    {
        if (message.getMessageId() == id)
            return true;
    }
    return false;
}

Message& Message::operator =(const Message& message)
{
    this->body=message.body;
    this->messageId=message.messageId;
    this->number=message.number;
    this->threadId=message.threadId;
    this->timeStamp=message.timeStamp;
    this->read=message.read;
    this->toa=message.toa;
    this->contactName=message.contactName;
    return *this;
}

bool MessageModel::clear()
{
    if (!this->messages.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->messages.size()-1);
        this->messages.clear();
        endRemoveRows();
        return true;
    }
    return false;
}

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[ThreadIdRole] = "threadId";
    roles[MessageIdRole] = "messageId";
    roles[TimeStampRole] = "timeStamp";
    roles[NumberRole] = "number";
    roles[BodyRole] = "body";
    roles[ReadRole] = "read";
    roles[ToaRole] = "toa";
    roles[ContactNameRole] = "contactName";
    setRoleNames(roles);
}

void MessageModel::addMessage(const Message &message)
{
    foreach(Message msg, this->messages)
    {
        if (msg.getMessageId() == message.getMessageId())
            return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    this->messages << message;
    endInsertRows();
}

void MessageModel::markThreadAsRead(QString id)
{
    Message message;

    for(int i = 0; i < this->messages.count() ; i++)
    {
        if (this->messages[i].getThreadId()==id)
        {
            message = this->messages[i];
            beginRemoveRows(QModelIndex(), i, i);
            this->messages.removeAt(i);
            endRemoveRows();
            addMessage(Message(id,message.getMessageId(),message.getTimeStamp(),message.getNumber(),"1",message.getToa(),message.getBody(),message.getContactName()));
        }
    }
}

int MessageModel::rowCount(const QModelIndex & parent) const
{
    return this->messages.count();
}

QVariant MessageModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > messages.count())
        return QVariant();

    const Message &message= this->messages[index.row()];
    if (role == ThreadIdRole)
        return message.getThreadId();
    else if (role == MessageIdRole)
        return message.getMessageId();
    else if (role == TimeStampRole)
        return message.getTimeStamp();
    else if (role == NumberRole)
        return message.getNumber();
    else if (role == BodyRole)
        return message.getBody();
    else if (role == ReadRole)
        return message.getRead();
    else if (role == ToaRole)
        return message.getToa();
    else if (role == ContactNameRole)
        return message.getContactName();
    return QVariant();
}
