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


#include "contactmodel.h"

Contact::Contact(const QString id, const QString number, const QString name)
{
    this->id = id;
    this->name = name;
    this->number = number;
    this->number.remove(QRegExp("\\s+$"));
}

QString Contact::getId() const
{
    return this->id;
}

QString Contact::getNumber() const
{
    return this->number;
}

QString Contact::getName() const
{
    return this->name;
}

QString ContactModel::getNumber(QString name)
{
    for (int i = 0 ; this->contacts.length() > i ; i++)
    {
        if (this->contacts.at(i).getName() == name)
            return this->contacts.at(i).getNumber();
    }
    return "error";
}

QString ContactModel::getName(QString number)
{
    for (int i = 0 ; this->contacts.length() > i ; i++)
    {
        QString tmp=number.remove(" ").right(9);
        QString tmp2=this->contacts.at(i).getNumber().remove(" ").right(9);
        if (tmp == tmp2)
            return this->contacts.at(i).getName();
    }
    return number;
}

bool ContactModel::clear()
{
    if (!this->contacts.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->contacts.size()-1);
        this->contacts.clear();
        endRemoveRows();
        return true;
    }
    return false;
}

ContactModel::ContactModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "threadId";
    roles[NumberRole] = "number";
    roles[NameRole] = "name";
    setRoleNames(roles);
}

void ContactModel::addContact(const Contact &contact)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    this->contacts << contact;
    endInsertRows();
}

int ContactModel::rowCount(const QModelIndex & parent) const
{
    return this->contacts.count();
}

QVariant ContactModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > contacts.count())
        return QVariant();

    const Contact &contact= this->contacts[index.row()];
    if (role == IdRole)
        return contact.getId();
    else if (role == NumberRole)
        return contact.getNumber();
    else if (role == NameRole)
        return contact.getName();
    return QVariant();
}
