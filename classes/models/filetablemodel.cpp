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


#include "filetablemodel.h"

File::File(QObject *parent)
    : QObject(parent)
{

}

File::File(const File& file)
{
    this->fileIcon = file.fileIcon;
    this->fileName = file.fileName;
    this->fileSize = file.fileSize;
    this->fileDate = file.fileDate;
    this->filePath = file.filePath;
    this->filePermissions = file.filePermissions;
    this->fileOwner = file.fileOwner;
    this->fileType = file.fileType;
    this->fileColor = file.fileColor;
}

File& File::operator =(const File& file)
{
    this->fileIcon = file.fileIcon;
    this->fileName = file.fileName;
    this->fileSize = file.fileSize;
    this->fileDate = file.fileDate;
    this->filePath = file.filePath;
    this->filePermissions = file.filePermissions;
    this->fileOwner = file.fileOwner;
    this->fileType = file.fileType;
    this->fileColor = file.fileColor;
    return *this;
}

FileTableModel::FileTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    this->coloring = false;
}

FileTableModel::FileTableModel(QList< File > fileList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->coloring = false;
    this->fileList = fileList;
}

int FileTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->fileList.size();
}

int FileTableModel::rowCount()
{
    return this->fileList.size();
}


int FileTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant FileTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->fileList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        int col = index.column();
        switch (col)
        {
        case 0:
            return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
        case 1:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 2:
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        case 3:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
         default:
            return QVariant();
        }
    }
    if (role == Qt::UserRole)
    {
        File file = this->fileList.at(index.row());
        return file.fileType;
    }
    if (role == 40)
    {
        File file = this->fileList.at(index.row());
        return file.filePath;
    }
    else if (role == Qt::DecorationRole)
    {
        File file = this->fileList.at(index.row());

        int col = index.column();
        if (col == 0)
        {
            return QVariant(file.fileIcon.pixmap(QSize(20, 20), QIcon::Normal));
        }
    }
    else if (role == Qt::DisplayRole)
    {
        File file = this->fileList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return "";
        case 1:
            return file.fileName;
        case 2:
            if (file.fileType == "dir")
                return "";
            else
                return FileTableModel::humanReadableSize(file.fileSize);
        case 3:
            return file.fileDate;
         default:
            return QVariant();
        }
    }
    else if (role == Qt::EditRole)
    {
        File file = this->fileList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return "";
        case 1:
            return file.fileName;
        case 2:
            if (file.fileType == "dir")
                return "";
            else
                return FileTableModel::humanReadableSize(file.fileSize);
        case 3:
            return file.fileDate;
         default:
            return QVariant();
        }
    }
    else if (role == Qt::ForegroundRole && this->coloring)
    {
        File file = this->fileList.at(index.row());
        return QVariant::fromValue(QBrush(file.fileColor, Qt::SolidPattern));
    }
    return QVariant();

}

int FileTableModel::getRow(QString fileName)
{
    for (int i = 0; i < this->fileList.size(); i++)
    {
        if (fileName == this->fileList.at(i).fileName)
        {
            return i;
        }
    }
    return -1;
}

QVariant FileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "";
        case 1:
            return tr("Name", "file table file name");
        case 2:
            return tr("Size", "file table file size");
        case 3:
            return tr("Date", "file table file date");
        default:
            return QVariant();
        }
    }
    return QVariant();

}

Qt::ItemFlags FileTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions FileTableModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

QStringList FileTableModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

QMimeData *FileTableModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    QStringList rowList;

    foreach (QModelIndex index, indexes) {
        if (index.isValid() && !rowList.contains(QString::number(index.row()))) {
            rowList.append(QString::number(index.row()));
//            QString text = data(index, 40).toString();
            QString text = QString::number(index.row());
            stream << text;
        }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool FileTableModel::dropMimeData(const QMimeData *data,
                                     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/vnd.text.list"))
        return false;

    if (column > 0)
        return false;
    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());
    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int rows = 0;

    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
        ++rows;
    }
//    insertRows(beginRow, rows, QModelIndex());
//    foreach (QString text, newItems) {
//        QModelIndex idx = index(beginRow, 0, QModelIndex());
//        setData(idx, text);
//        beginRow++;
//    }
    emit this->copy(newItems);
    return true;
}

bool FileTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        File file = this->fileList.value(row);

        int col = index.column();
        switch (col)
        {
        case 0:
            file.fileIcon = value.value<QIcon>();
            break;
        case 1:
            file.fileName = value.toString();
            break;
        case 2:
            file.fileSize = value.toString();
            break;
        case 3:
            file.fileDate = value.toString();
            break;
        default:
            return false;
            break;
        }

        this->fileList.replace(row, file);
        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

bool FileTableModel::insertFile(int position, File file)
{
    beginInsertRows(QModelIndex(), position, position);

    this->fileList.insert(position, file);

    endInsertRows();
    return true;
}

bool FileTableModel::insertFiles(int position, QList<File> tmpList)
{
    beginInsertRows(QModelIndex(), position, position+tmpList.size()-1);

    while (!tmpList.isEmpty())
    {
        this->fileList.insert(position, tmpList.takeFirst());
    }

    endInsertRows();
    return true;
}

bool FileTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        File file;
        file.fileIcon = QIcon();
        file.fileName = "";
        file.fileSize = "";
        file.fileDate = "";

        this->fileList.insert(position, file);
    }

    endInsertRows();
    return true;

}


bool FileTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
       this->fileList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

File FileTableModel::getFile(int row)
{
    return this->fileList.value(row);
}

QList< File > FileTableModel::getList()
{
    return this->fileList;
}

bool FileTableModel::clear()
{
    if (!this->fileList.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->fileList.size()-1);

        this->fileList.clear();

        endRemoveRows();

        return true;
    }
    return false;
}

QString FileTableModel::humanReadableSize(QString size)
{
    long double sizeTmp = 0.0;
    QString strTmp = " B";
    sizeTmp = size.toLongLong();
    if (sizeTmp <= 1024)
    {
        strTmp.prepend(QString::number(sizeTmp, 'f', 0));
        return strTmp;
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " KiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " MiB";
    }
    if (sizeTmp > 1024)
    {
        sizeTmp = sizeTmp / 1024;
        strTmp = " GiB";
    }
    strTmp.prepend(QString::number(sizeTmp, 'f', 2));
    return strTmp;
}

bool FileTableModel::setColoring(bool enable)
{
    this->coloring = enable;
    return true;
}

FileSortModel::FileSortModel(QObject *parent)
        : QSortFilterProxyModel(parent)
{

}

bool FileSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
        QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);

        return sourceModel()->data(index1).toString().contains(filterRegExp());
}

bool FileSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QString leftType = sourceModel()->data(left, Qt::UserRole).toString();
    QString leftName = sourceModel()->data(left, Qt::DisplayRole).toString();

    QString rightType = sourceModel()->data(right, Qt::UserRole).toString();
    QString rightName = sourceModel()->data(right, Qt::DisplayRole).toString();

    if (leftType == "dir" && rightType != "dir"){
        return true;
    }
    else if (leftType != "dir" && rightType == "dir")
    {
        return false;
    }
    else
    {
        if (leftName.contains(QRegExp("B$")) && rightName.contains(QRegExp("B$")))
        {
            double sizeThis, sizeItem;
            sizeThis = leftName.left(leftName.indexOf(' ')).toDouble();
            sizeItem = rightName.left(rightName.indexOf(' ')).toDouble();
            bool wynik = sizeThis<sizeItem;
            if (leftName.contains(QRegExp("^\\d+ B$")))
            {
                if (rightName.contains(QRegExp("^\\d+ B$")))
                    return wynik;
                if (rightName.contains(QRegExp("^\\d+.\\d+ KiB$")))
                    return true;
                if (rightName.contains(QRegExp("^\\d+.\\d+ MiB$")))
                    return true;
                if (rightName.contains(QRegExp("^\\d+.\\d+ GiB$")))
                    return true;
            }
            if (leftName.contains(QRegExp("^\\d+.\\d+ KiB$")))
            {
                if (rightName.contains(QRegExp("^\\d+ B$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ KiB$")))
                    return wynik;
                if (rightName.contains(QRegExp("^\\d+.\\d+ MiB$")))
                    return true;
                if (rightName.contains(QRegExp("^\\d+.\\d+ GiB$")))
                    return true;
            }
            if (leftName.contains(QRegExp("^\\d+.\\d+ MiB$")))
            {
                if (rightName.contains(QRegExp("^\\d+ B$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ KiB$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ MiB$")))
                    return wynik;
                if (rightName.contains(QRegExp("^\\d+.\\d+ GiB$")))
                    return true;
            }
            if (leftName.contains(QRegExp("^\\d+.\\d+ GiB$")))
            {
                if (rightName.contains(QRegExp("^\\d+ B$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ KiB$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ MiB$")))
                    return false;
                if (rightName.contains(QRegExp("^\\d+.\\d+ GiB$")))
                    return wynik;
            }
            return leftName.toLower() < rightName.toLower();
        }
        else if (this->sortCaseSensitivity() == Qt::CaseSensitive)
            return leftName < rightName;
        else
            return leftName.toLower() < rightName.toLower();
    }
}

