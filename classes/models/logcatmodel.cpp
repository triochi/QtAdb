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


#include "logcatmodel.h"

LogcatMessage::LogcatMessage(QObject *parent)
    : QObject(parent)
{

}

LogcatMessage::LogcatMessage(const LogcatMessage& logcatMessage)
{
    this->type = logcatMessage.type;
    this->pid = logcatMessage.pid;
    this->message = logcatMessage.message;
    this->sender = logcatMessage.sender;
    this->timestamp = logcatMessage.timestamp;
//    this->number = logcatMessage.number;
}

LogcatMessage& LogcatMessage::operator =(const LogcatMessage& logcatMessage)
{
    this->type = logcatMessage.type;
    this->pid = logcatMessage.pid;
    this->message = logcatMessage.message;
    this->sender = logcatMessage.sender;
    this->timestamp = logcatMessage.timestamp;
//    this->number = logcatMessage.number;
    return *this;
}

LogcatModel::LogcatModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

LogcatModel::LogcatModel(QList< LogcatMessage > logcatList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->logcatList = logcatList;
}

int LogcatModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->logcatList.size();
}

LogcatMessage LogcatModel::getRow(int row)
{
    return this->logcatList.value(row);
}

int LogcatModel::rowCount()
{
    return this->logcatList.size();
}


int LogcatModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant LogcatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->logcatList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        int col = index.column();
        switch (col)
        {
        case 0:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 1:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 2:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 3:
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        case 4:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
         default:
            return QVariant();
        }
    }
    else if (role == Qt::DisplayRole)
    {
        LogcatMessage logcatMessage = this->logcatList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return logcatMessage.timestamp;
        case 1:
            return logcatMessage.type;
        case 2:
            return logcatMessage.sender;
        case 3:
            return logcatMessage.pid;
        case 4:
            return logcatMessage.message;
         default:
            return QVariant();
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        LogcatMessage logcatMessage = this->logcatList.at(index.row());
        if (logcatMessage.type == "Debug")
            return QVariant::fromValue(QBrush(Qt::darkBlue, Qt::SolidPattern));
        else if (logcatMessage.type == "Error")
            return QVariant::fromValue(QBrush(Qt::red, Qt::SolidPattern));
        else if (logcatMessage.type == "Info")
            return QVariant::fromValue(QBrush(Qt::darkGreen, Qt::SolidPattern));
        else if (logcatMessage.type == "Warning")
            return QVariant::fromValue(QBrush(QColor(255,165,0), Qt::SolidPattern));
        else
            return QVariant::fromValue(QBrush(Qt::black, Qt::SolidPattern));
    }
    return QVariant();

}

QVariant LogcatModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Time", "logcat header");
        case 1:
            return tr("Type", "logcat header");
        case 2:
            return tr("Sender", "logcat header");
        case 3:
            return tr("Pid", "logcat header");
        case 4:
            return tr("Message", "logcat header");
        default:
            return QVariant();
        }
    }
    return QVariant();

}

Qt::ItemFlags LogcatModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);

}

bool LogcatModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        LogcatMessage logcatMessage = this->logcatList.value(row);

        int col = index.column();
        switch (col)
        {
        case 0:
            break;
//            logcatMessage.number = value.toString();
        case 1:
            logcatMessage.type = value.toString();
            break;
        case 2:
            logcatMessage.sender = value.toString();
            break;
        case 3:
            logcatMessage.pid = value.toString();
            break;
        case 4:
            logcatMessage.message = value.toString();
            break;
        default:
            return false;
            break;
        }

        this->logcatList.replace(row, logcatMessage);
        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

bool LogcatModel::insertMessage(int position, LogcatMessage logcatMessage)
{
    beginInsertRows(QModelIndex(), position, position);

    this->logcatList.insert(position, logcatMessage);

    endInsertRows();
    return true;
}

bool LogcatModel::insertMessage(LogcatMessage logcatMessage)
{
    beginInsertRows(QModelIndex(), logcatList.size(), logcatList.size());

    this->logcatList.append(logcatMessage);

    endInsertRows();
    return true;
}

bool LogcatModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        LogcatMessage logcatMessage;
        logcatMessage.type = "";
        logcatMessage.sender = "";
        logcatMessage.pid = "";
        logcatMessage.message = "";
//        logcatMessage.number = "";

        this->logcatList.insert(position, logcatMessage);
    }

    endInsertRows();
    return true;

}


bool LogcatModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
       this->logcatList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QList< LogcatMessage > LogcatModel::getList()
{
    return this->logcatList;
}

bool LogcatModel::clear()
{
    if (!this->logcatList.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->logcatList.size()-1);

        this->logcatList.clear();

        endRemoveRows();

        return true;
    }
    return false;
}


SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    int i;
    for (i = 0; i < 5; i++)
        this->regExp.append(QRegExp());
}

void SortFilterProxyModel::addFilterRegExp(qint32 column, const QRegExp &pattern)
{
    this->regExp[column] = pattern;
    invalidateFilter();
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(regExp.isEmpty())
        return true;

    bool ret = false;

    int i;
    for(i = 0; i < this->columnCount(QModelIndex()); i++)
    {
        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString str = index.data().toString();
        QString pat = regExp.at(i).pattern();
        ret = (str.contains(regExp.at(i)));

        if(!ret)
            return ret;
    }

    return ret;
}

void SortFilterProxyModel::clearFilters()
{
    this->regExp.clear();
    int i;
    for (i = 0; i < 5; i++)
        this->regExp.append(QRegExp());
}
