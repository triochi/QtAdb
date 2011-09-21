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


#ifndef MESSAGETHREADMODEL_H
#define MESSAGETHREADMODEL_H

#include "messagemodel.h"

class MessageThread
{
public:
    MessageThread(QString id, QString number, QString timeStamp, QString read, QString lastBody, QString messageCount, QString contactName);
    MessageThread();
    QString getId() const;
    QString getNumber() const;
    QString getTimeStamp() const;
    QString getLastBody() const;
    QString getMessageCount() const;
    QString getRead() const;
    QString getContactName() const;
    MessageThread& operator =(const MessageThread&);

private:
    QString id;
    QString number;
    QString timeStamp;
    QString lastBody;
    QString messageCount;
    QString read;
    QString contactName;
};


class MessageThreadModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MessageThreadModel(QObject *parent = 0);
    bool exists(QString threadID);
    void updateMessageCount(QString id);
    void markAsRead(QString id);
    MessageThread getThread(QString threadId);
    QString getThreadID(QString number);
    enum MessageThreadRoles { ThreadIdRole,MessageCountRole,TimeStampRole,NumberRole,LastBodyRole, ReadRole, ContactNameRole};
    void updateThread(QString id, QString timestamp,QString lastBody, QString messageCount, QString read);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void addThread(const MessageThread &thread);
    bool clear();

private:
    QList<MessageThread> threads;
};

#endif // MESSAGETHREADMODEL_H
