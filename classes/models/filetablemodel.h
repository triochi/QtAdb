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


#ifndef FILETABLEMODEL_H
#define FILETABLEMODEL_H

#include <QtGui>
//#include "../dialogs/appinfo.h"

class File : public QObject{
public:
    File(QObject *parent=0);
    File(const File&);
    QIcon fileIcon;
    QString fileName;
    QString fileSize;
    QString fileDate;
    QString filePath;
    QString filePermissions;
    QString fileOwner;
    QString fileType;
    QColor fileColor;
    File& operator =(const File&);
};


Q_DECLARE_METATYPE(File)

class FileTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileTableModel(QObject *parent=0);
    FileTableModel(QList< File > fileList, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertFile(int position, File file);
    bool insertFiles(int position, QList< File > fileList);
    File getFile(int row);
    int getRow(QString fileName);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    QList< File > getList();
    bool clear();
    static QString humanReadableSize(QString size);
    bool setColoring(bool);

    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
private:
    QList< File > fileList;
    bool coloring;
signals:
    void copy(QStringList list);
};

class FileSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    FileSortModel(QObject *parent = 0);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
     bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // PHONEFILETABLEMODEL_H
