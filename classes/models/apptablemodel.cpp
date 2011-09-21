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


#include "apptablemodel.h"

App::App(QObject *parent)
    : QObject(parent)
{

}

App::App(const App& app)
{
    this->appFile = app.appFile;
    this->appFileName = app.appFileName;
    this->appIcon = app.appIcon;
    this->appName = app.appName;
    this->appSize = app.appSize;
    this->appVersion = app.appVersion;
    this->packageName = app.packageName;
    this->icoName = app.icoName;
    this->location = app.location;
    this->qrCode = app.qrCode;
    this->cyrketVer = app.cyrketVer;
    this->date = app.date;
}

App& App::operator =(const App& app)
{
    this->appFile = app.appFile;
    this->appFileName = app.appFileName;
    this->appIcon = app.appIcon;
    this->appName = app.appName;
    this->appSize = app.appSize;
    this->appVersion = app.appVersion;
    this->packageName = app.packageName;
    this->icoName = app.icoName;
    this->location = app.location;
    this->qrCode = app.qrCode;
    this->cyrketVer = app.cyrketVer;
    this->date = app.date;
    return *this;
}

AppTableModel::AppTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

AppTableModel::AppTableModel(QList< App > appList, QObject *parent)
    : QAbstractTableModel(parent)
{
    this->appList = appList;
}

int AppTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->appList.size();
}

int AppTableModel::rowCount()
{
    return this->appList.size();
}

int AppTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7;
}

QVariant AppTableModel::data(const QModelIndex &index, int role) const
{
    QString tmp;
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->appList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DecorationRole)
    {
        App app = this->appList.at(index.row());

        int col = index.column();
        if (col == 0)
        {
//            return QVariant(app.qrCode);
            return QVariant(app.appIcon.pixmap(QSize(40, 40), QIcon::Normal));
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
        App app = this->appList.at(index.row());

        int col = index.column();
        switch (col)
        {
        case 0:
            return "";
        case 1:
            return app.appName;
        case 2:
            tmp = app.appVersion;
            if (tmp.length()>15)
                tmp = tmp.left(15)+"...";
            return tmp;
        case 3:
            return AppTableModel::humanReadableSize(app.appSize);
        case 4:
            return app.packageName;
        case 5:
            return app.appFile;
        case 6:
            return app.location;
         default:
            return QVariant();
        }
    }
    else if (role == Qt::BackgroundColorRole)
    {
        App app = this->appList.at(index.row());
        if (app.appVersion < app.cyrketVer)
            return QVariant::fromValue(QBrush(Qt::green, Qt::SolidPattern));
    }
    return QVariant();

}

QVariant AppTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "";
        case 1:
            return tr("Name", "application table name");
        case 2:
            return tr("Version", "application table version");
        case 3:
            return tr("Size", "applications table size");
        case 4:
            return tr("Package", "application table package");
        case 5:
            return tr("File", "application table file");
        case 6:
            return tr("Location", "application table location");
        default:
            return QVariant();
        }
    }
    return QVariant();

}

Qt::ItemFlags AppTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

}

bool AppTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();

        App app = this->appList.value(row);

        int col = index.column();
        switch (col)
        {
        case 0:
            app.appIcon = value.value<QIcon>();
            break;
        case 1:
            app.appName = value.toString();
            break;
        case 2:
            app.appVersion = value.toString();
            break;
        case 3:
            app.appSize = value.toString();
            break;
        case 4:
            app.packageName = value.toString();
            break;
        case 5:
            app.appFile = value.toString();
            break;
        case 6:
            app.location = value.toString();
            break;
        default:
            return false;
            break;
        }

        this->appList.replace(row, app);
        emit(dataChanged(index, index));

        return true;
    }

    return false;

}

bool AppTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        App app;
        app.appIcon = QIcon();
        app.appName = "";
        app.appVersion = "";
        app.appSize = "";
        app.packageName = "";
        app.appFile = "";
        app.location = "";

        this->appList.insert(position, app);
    }

    endInsertRows();
    return true;

}
bool AppTableModel::insertApp(int position, App app)
{
    beginInsertRows(QModelIndex(), position, position);

    this->appList.insert(position, app);

    endInsertRows();
    return true;
}

bool AppTableModel::insertApps(int position, QList< App > tmpList)
{
    beginInsertRows(QModelIndex(), position, position+tmpList.size()-1);

    while (!tmpList.isEmpty())
    {
        this->appList.insert(position, tmpList.takeFirst());
    }

    endInsertRows();
    return true;
}

App AppTableModel::getApp(int row)
{
    return this->appList.at(row);
}

void AppTableModel::setQr(int row, QPixmap pix)
{
    App app = this->appList.at(row);
    app.qrCode = pix;
    this->appList.replace(row, app);
//    for (int i=0; i<this->appList.size(); i++)
//    {
//        App app = this->appList.at(i);
//        if (app.packageName == package)
//        {
//            app.qrCode = pix;
//            this->appList.replace(i, app);
//            return;
//        }
//    }
}

void AppTableModel::setCyrketVer(int i, QString str)
{
    App app = this->appList.at(i);
    app.cyrketVer = str;
    this->appList.replace(i, app);
//    for (int i=0; i<this->appList.size(); i++)
//    {
//        App app = this->appList.at(i);
//        if (app.packageName == package)
//        {
//            app.cyrketVer = str;
//            this->appList.replace(i, app);
//            return;
//        }
//    }
}

QList< App > AppTableModel::getList()
{
    return this->appList;
}

bool AppTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
       this->appList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

bool AppTableModel::clear()
{
    if (!this->appList.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, this->appList.size()-1);

        this->appList.clear();

        endRemoveRows();

        return true;
    }
    return false;
}

QString AppTableModel::humanReadableSize(QString size)
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

bool AppTableModel::isEmpty()
{
    if (this->appList.size() == 0)
        return true;
    else
        return false;
}

AppSortModel::AppSortModel(QObject *parent)
        : QSortFilterProxyModel(parent)
{

}

bool AppSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
        QModelIndex index1 = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);

        return sourceModel()->data(index1).toString().contains(filterRegExp());
}

bool AppSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
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
    {
        return leftName < rightName;
    }
    else
    {
        return leftName.toLower() < rightName.toLower();
    }
}
