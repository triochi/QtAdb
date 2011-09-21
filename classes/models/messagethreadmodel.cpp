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


#include "messagethreadmodel.h"

MessageThread::MessageThread(QString id, QString number, QString timeStamp,QString read, QString lastBody, QString messageCount, QString contactName)
{
    this->id=id;
    this->number=number;
    this->timeStamp=timeStamp;
    this->lastBody=lastBody;
    this->messageCount=messageCount;
    this->read=read;
    this->contactName=contactName;
}

MessageThread::MessageThread()
{
}

QString MessageThread::getId() const
{
    return this->id;
}

QString MessageThread::getNumber() const
{
    return this->number;
}

QString MessageThread::getTimeStamp() const
{
    return this->timeStamp;
}

QString MessageThread::getLastBody() const
{
    return this->lastBody;
}

QString MessageThread::getMessageCount() const
{
    return this->messageCount;
}

QString MessageThread::getRead() const
{
    return this->read;
}

QString MessageThread::getContactName() const
{
    return this->contactName;
}

MessageThread& MessageThread::operator =(const MessageThread& thread)
{
    this->id=thread.getId();
    this->number=thread.getNumber();
    this->timeStamp=thread.getTimeStamp();
    this->lastBody=thread.getLastBody();
    this->messageCount=thread.getMessageCount();
    this->read=thread.getRead();
    this->contactName=thread.getContactName();
    return *this;
}

void MessageThreadModel::markAsRead(QString id)
{
    int i;
    MessageThread thread;

    for(i = 0; i < this->threads.count() ; i++)
    {
        if (this->threads[i].getId()==id)
        {
            thread = this->threads[i];
            break;
        }
    }

    beginRemoveRows(QModelIndex(), i, i);
    this->threads.removeAt(i);
    endRemoveRows();

    addThread(MessageThread(id,thread.getNumber(),thread.getTimeStamp(),"1",thread.getLastBody(),thread.getMessageCount(),thread.getContactName()));
}

MessageThread MessageThreadModel::getThread(QString threadId)
{
    threadId.remove("^");
    threadId.remove("$");
    foreach (MessageThread thread, this->threads)
    {
        if (thread.getId() == threadId)
            return thread;
    }
    return MessageThread();
}

QString MessageThreadModel::getThreadID(QString number)
{
    /*
    foreach (MessageThread thread, this->threads)
    {
        if (thread.getId() == threadId)
            return thread;
    }
    return MessageThread();
    */
}

void MessageThreadModel::updateThread(QString id, QString timestamp,QString lastBody, QString messageCount, QString read)
{
    int i;
    MessageThread thread;

    for(i = 0; i < this->threads.count() ; i++)
    {
        if (this->threads[i].getId()==id)
        {
            thread = this->threads[i];
            break;
        }
    }

    beginRemoveRows(QModelIndex(), i, i);
    this->threads.removeAt(i);
    endRemoveRows();

    addThread(MessageThread(id,thread.getNumber(),timestamp,read,lastBody,QString::number(thread.getMessageCount().toInt()+1),thread.getContactName()));
}

bool MessageThreadModel::exists(QString threadID)
{
    foreach(MessageThread thread,this->threads)
    {
        if (thread.getId() == threadID)
            return true;
    }
    return false;
}

bool MessageThreadModel::clear()
{
    if (!this->threads.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->threads.size()-1);
        this->threads.clear();
        endRemoveRows();
        return true;
    }
    return false;
}

MessageThreadModel::MessageThreadModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[ThreadIdRole] = "threadId";
    roles[MessageCountRole] = "messageCount";
    roles[TimeStampRole] = "timeStamp";
    roles[NumberRole] = "number";
    roles[LastBodyRole] = "lastBody";
    roles[ReadRole] = "read";
    roles[ContactNameRole] = "contactName";
    setRoleNames(roles);
}

void MessageThreadModel::addThread(const MessageThread &thread)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    this->threads << thread;
    endInsertRows();
}

int MessageThreadModel::rowCount(const QModelIndex & parent) const
{
    return this->threads.count();
}

QVariant MessageThreadModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > threads.count())
        return QVariant();

    const MessageThread &thread= this->threads[index.row()];
    if (role == ThreadIdRole)
        return thread.getId();
    else if (role == MessageCountRole)
        return thread.getMessageCount();
    else if (role == TimeStampRole)
        return thread.getTimeStamp();
    else if (role == NumberRole)
        return thread.getNumber();
    else if (role == LastBodyRole)
        return thread.getLastBody();
    else if (role == ReadRole)
        return thread.getRead();
    else if (role == ContactNameRole)
        return thread.getContactName();
    return QVariant();
}
