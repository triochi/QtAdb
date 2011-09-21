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


#ifndef LOGCATMODEL_H
#define LOGCATMODEL_H

#include <QtGui>

class LogcatMessage : public QObject{
public:
    LogcatMessage(QObject *parent=0);
    LogcatMessage(const LogcatMessage&);
    QString type;
    QString pid;
    QString sender;
    QString message;
    QString timestamp;
//    QString number;
    LogcatMessage& operator =(const LogcatMessage&);
};


Q_DECLARE_METATYPE(LogcatMessage)

class LogcatModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    LogcatModel(QObject *parent=0);
    LogcatModel(QList< LogcatMessage > fileList, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertMessage(int position, LogcatMessage logcatMessage);
    bool insertMessage(LogcatMessage logcatMessage);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    LogcatMessage getRow(int row);
    QList< LogcatMessage > getList();
    bool clear();

private:
    QList< LogcatMessage > logcatList;

};

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QObject *parent = 0);
    void addFilterRegExp(qint32 column, const QRegExp &pattern);
    void clearFilters();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QList<QRegExp> regExp;
};
#endif // LOGCATMODEL_H
