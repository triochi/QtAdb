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


#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>

class Message
{
public:
    Message( const QString threadId, const QString messageId, const QString timeStamp, const QString number,
            const QString read, const QString toa, const QString body, const QString contactName);
    Message();

    QString getThreadId() const;
    QString getMessageId() const;
    QString getTimeStamp() const;
    QString getNumber() const;
    QString getBody() const;
    QString getRead() const;
    QString getToa() const;
    QString getContactName() const;
    Message& operator =(const Message&);
private:
    QString threadId;
    QString messageId;
    QString timeStamp;
    QString number;
    QString body;
    QString read;
    QString toa;
    QString contactName;
};

class MessageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MessageModel(QObject *parent = 0);

    enum MessageRoles { ThreadIdRole,MessageIdRole,TimeStampRole,NumberRole,BodyRole, ReadRole, ToaRole, ContactNameRole };
    bool isMessageExists(QString id);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void addMessage(const Message &message);
    void markThreadAsRead(QString id);
    bool clear();

private:
    QList<Message> messages;
};


#endif // MESSAGEMODEL_H
