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


#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractListModel>
#include <QRegExp>

class Contact
{
public:
    Contact( const QString id, const QString number, const QString name);

    QString getId() const;
    QString getNumber() const;
    QString getName() const;
private:
    QString id;
    QString number;
    QString name;
};

class ContactModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ContactModel(QObject *parent = 0);

    enum ContactRoles { IdRole, NameRole, NumberRole };

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void addContact(const Contact &contact);
    bool clear();
    QString getNumber(QString name);
    QString getName(QString number);

private:
    QList<Contact> contacts;
};

#endif // CONTACTMODEL_H
