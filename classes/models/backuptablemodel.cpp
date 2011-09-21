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


#include "backuptablemodel.h"

Backup::Backup(QObject *parent)
    : QObject(parent)
{

}

Backup::Backup(const Backup& backup)
{
    this->appIcon = backup.appIcon;
    this->appName = backup.appName;
    this->appSize = backup.appSize;
    this->appVersion = backup.appVersion;
    this->withData = backup.withData;
    this->withApk = backup.withApk;
    this->packageName = backup.packageName;

}

Backup& Backup::operator =(const Backup& backup)
{
    this->appIcon = backup.appIcon;
    this->appName = backup.appName;
    this->appSize = backup.appSize;
    this->appVersion = backup.appVersion;
    this->withData = backup.withData;
    this->withApk = backup.withApk;
    this->packageName = backup.packageName;
    return *this;
}

BackupTableModel::BackupTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

BackupTableModel::BackupTableModel(QList< Backup > backupList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->backupList = backupList;
}

int BackupTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->backupList.size();
}

int BackupTableModel::rowCount()
{
    return this->backupList.size();
}

int BackupTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant BackupTableModel::data(const QModelIndex &index, int role) const
{
    QString tmp;
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->backupList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        Backup backup = this->backupList.at(index.row());

        int col = index.column();
        if (col == 0)
        {
            return QVariant(backup.appIcon.pixmap(QSize(40, 40), QIcon::Normal));
        }
        else
            return QVariant();
    }
    else if (role == Qt::TextAlignmentRole)
    {
        int col = index.column();
        switch (col)
        {
        case 0:
            return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
        case 1:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 2:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 3:
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        case 4:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 5:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        case 6:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
         default:
            return QVariant();
        }
    }
    else if (role == Qt::DisplayRole) {
        Backup backup = this->backupList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return "";
        case 1:
            return backup.appName;
        case 2:
            tmp = backup.appVersion;
            if (tmp.length()>15)
                tmp = tmp.left(15)+"...";
            return tmp;
        case 3:
            return BackupTableModel::humanReadableSize(backup.appSize);
        case 4:
            return backup.packageName;
        case 5:
            if (backup.withApk)
                return QString(tr("yes"));
            else
                return QString(tr("no"));
        case 6:
            if (backup.withData)
                return QString(tr("yes"));
            else
                return QString(tr("no"));
         default:
            return QVariant();
        }
    }
    return QVariant();

}

QVariant BackupTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "";
        case 1:
            return tr("Name", "backup table header app name");
        case 2:
            return tr("Version", "backup table header app version");
        case 3:
            return tr("Size", "backup table header app size");
        case 4:
            return tr("Package", "backup table header app package");
        case 5:
            return tr("App", "backup table header apk file");
        case 6:
            return tr("Data", "backup table header data");
        default:
            return QVariant();
        }
    }
    return QVariant();

}

Qt::ItemFlags BackupTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

}

bool BackupTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        Backup backup = this->backupList.value(row);

        int col = index.column();
        switch (col)
        {
        case 0:
            backup.appIcon = value.value<QIcon>();
            break;
        case 1:
            backup.appName = value.toString();
            break;
        case 2:
            backup.appVersion = value.toString();
            break;
        case 3:
            backup.appSize = value.toString();
            break;
        case 4:
            backup.packageName = value.toString();
            break;
        case 5:
            backup.withApk = value.toBool();
            break;
        case 6:
            backup.withData = value.toBool();
            break;
        default:
            return false;
            break;
        }

        this->backupList.replace(row, backup);
        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

bool BackupTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        Backup backup;
        backup.appIcon = QIcon();
        backup.appName = "";
        backup.appVersion = "";
        backup.appSize = "";
        backup.packageName = "";
        backup.withApk = false;
        backup.withData = false;

        this->backupList.insert(position, backup);
    }

    endInsertRows();
    return true;

}
bool BackupTableModel::insertBackup(int position, Backup backup)
{
    beginInsertRows(QModelIndex(), position, position);

    this->backupList.insert(position, backup);

    endInsertRows();
    return true;
}

bool BackupTableModel::insertBackups(int position, QList< Backup > tmpList)
{
    beginInsertRows(QModelIndex(), position, position+tmpList.size()-1);

    while (!tmpList.isEmpty())
    {
        this->backupList.insert(position, tmpList.takeFirst());
    }

    endInsertRows();
    return true;
}

Backup BackupTableModel::getBackup(int row)
{
    return this->backupList.at(row);
}

QList< Backup > BackupTableModel::getList()
{
    return this->backupList;
}

bool BackupTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
       this->backupList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

bool BackupTableModel::clear()
{
    if (!this->backupList.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->backupList.size()-1);

        this->backupList.clear();

        endRemoveRows();

        return true;
    }
    return false;
}

QString BackupTableModel::humanReadableSize(QString size)
{
    if (size.contains(QRegExp("^\\d+.\\d+ .iB$")) || size.contains(QRegExp("^\\d+ B$")))
    {
        return size;
    }
    else
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
}

bool BackupTableModel::isEmpty()
{
    if (this->backupList.size() == 0)
        return true;
    else
        return false;
}

BackupSortModel::BackupSortModel(QObject *parent)
        : QSortFilterProxyModel(parent)
{

}

bool BackupSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
        QModelIndex index1 = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);

        return sourceModel()->data(index1).toString().contains(filterRegExp());
}

bool BackupSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QString leftName = sourceModel()->data(left, Qt::DisplayRole).toString();

    QString rightName = sourceModel()->data(right, Qt::DisplayRole).toString();

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
