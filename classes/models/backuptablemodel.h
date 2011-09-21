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


#ifndef BACKUPTABLEMODEL_H
#define BACKUPTABLEMODEL_H

#include <QtGui>

class Backup : public QObject
{
//    Q_OBJECT

public:
    Backup(QObject *parent=0);
    Backup(const Backup&);
    QIcon appIcon;
    QString appName;
    QString appSize;
    QString appVersion;
    bool withData;
    bool withApk;
    QString packageName;

    Backup& operator =(const Backup&);
};// Backup;

Q_DECLARE_METATYPE(Backup)

class BackupTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    BackupTableModel(QObject *parent=0);
    BackupTableModel(QList< Backup > backupList, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool insertBackup(int position, Backup backup);
    bool insertBackups(int position, QList< Backup > backupList);
    Backup getBackup(int row);
    QList< Backup > getList();
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool clear();
    static QString humanReadableSize(QString size);
    bool isEmpty();

private:
    QList< Backup > backupList;
};


class BackupSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    BackupSortModel(QObject *parent = 0);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
     bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // BACKUPTABLEMODEL_H
